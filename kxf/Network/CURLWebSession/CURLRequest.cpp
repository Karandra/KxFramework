#include "KxfPCH.h"
#include "CURLRequest.h"
#include "CURLSession.h"
#include "CURL.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/MemoryStream.h"
#include "kxf/System/HandlePtr.h"
#include "kxf/General/Enumerator.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Utility/Container.h"

namespace
{
	using CharTraits = std::char_traits<char>;

	void NormalizeValue(kxf::String& value)
	{
		value.Trim();
		value.Trim(kxf::StringOpFlag::FromEnd);
		value.Replace(wxS("\r"), kxf::NullString);
		value.Replace(wxS("\n"), kxf::NullString);
	}
	kxf::String GetHeaderName(const char* source, size_t length)
	{
		if (source && length != 0)
		{
			if (const char* colon = CharTraits::find(source, length, ':'))
			{
				auto value = kxf::String::FromUTF8(source, colon - source);
				NormalizeValue(value);

				return value;
			}
		}
		return {};
	}
	kxf::String GetHeaderValue(const char* source, size_t length)
	{
		if (source && length != 0)
		{
			if (const char* colon = CharTraits::find(source, length, ':'))
			{
				// Skip colon itself and a single space after it
				constexpr size_t offset = 2;

				auto value = kxf::String::FromUTF8(colon + offset, length - (colon - source) - offset);
				NormalizeValue(value);

				return value;
			}
		}
		return {};
	}
}

namespace kxf
{
	void CURLRequest::NotifyEvent(EventTag<WebRequestEvent> eventID, WebRequestEvent& event)
	{
		if (!ProcessEvent(event, eventID, ProcessEventFlag::HandleExceptions) || event.IsSkipped())
		{
			m_Session.ProcessEvent(event, eventID, ProcessEventFlag::HandleExceptions);
		}
	}

	bool CURLRequest::OnCallbackCommon(bool isWrite, size_t& result)
	{
		if (m_NextState == WebRequestState::Cancelled)
		{
			m_NextState = WebRequestState::None;

			result = isWrite ? 0 : CURL_READFUNC_ABORT;
			return true;
		}
		else if (m_NextState == WebRequestState::Paused)
		{
			m_NextState = WebRequestState::None;
			NotifyStateChange(WebRequestState::Paused);

			result = isWrite ? CURL_WRITEFUNC_PAUSE : CURL_READFUNC_PAUSE;
			return true;
		}
		return false;
	}
	size_t CURLRequest::OnReadData(char* data, size_t size, size_t count)
	{
		size_t result = 0;
		if (OnCallbackCommon(false, result))
		{
			return result;
		}
		else if (m_SendStream)
		{
			const size_t length = size * count;
			m_SendStream->Read(data, length);

			WebRequestEvent event(*this, m_State, data, length);
			NotifyEvent(WebRequestEvent::EvtDataSent, event);

			return m_SendStream->LastRead().ToBytes();
		}
		return 0;
	}
	size_t CURLRequest::OnWriteData(char* data, size_t size, size_t count)
	{
		size_t result = 0;
		if (OnCallbackCommon(false, result))
		{
			return result;
		}
		else if (m_ReceiveStream)
		{
			const size_t length = size * count;
			m_ReceiveStream->Write(data, size * count);

			WebRequestEvent event(*this, m_State, data, length);
			NotifyEvent(WebRequestEvent::EvtDataReceived, event);

			return m_ReceiveStream->LastWrite().ToBytes();
		}
		return 0;
	}
	size_t CURLRequest::OnReceiveHeader(char* data, size_t size, size_t count)
	{
		const size_t length = size * count;

		WebRequestHeader header(GetHeaderName(data, length), GetHeaderValue(data, length));
		if (header)
		{
			WebRequestEvent event(*this, m_State, std::move(header));
			NotifyEvent(WebRequestEvent::EvtHeaderReceived, event);
		}
		return length;
	}
	int CURLRequest::OnProgressNotify(int64_t bytesReceived, int64_t bytesExpectedToReceive, int64_t bytesSent, int64_t bytesExpectedToSend)
	{
		// Update sizes
		m_BytesReceived = bytesReceived;
		m_BytesExpectedToReceive = bytesExpectedToReceive;
		m_BytesSent = bytesSent;
		m_BytesExpectedToSend = bytesExpectedToSend;

		// Handle pause and cancellation
		if (m_NextState == WebRequestState::Resumed)
		{
			m_NextState = WebRequestState::None;
			::curl_easy_pause(m_Handle.GetNativeHandle(), CURLPAUSE_CONT);
			m_State = WebRequestState::Active;

			NotifyStateChange(WebRequestState::Resumed);
		}
		else if (m_NextState == WebRequestState::Cancelled)
		{
			m_NextState = WebRequestState::None;
			return -1;
		}
		return CURL_PROGRESSFUNC_CONTINUE;
	}

	void CURLRequest::DoFreeRequestHeaders()
	{
		if (m_RequestHeadersSList)
		{
			m_Handle.SetOption(CURLOPT_HTTPHEADER, nullptr);

			::curl_slist_free_all(reinterpret_cast<curl_slist*>(m_RequestHeadersSList));
			m_RequestHeadersSList = nullptr;
		}
	}
	void CURLRequest::DoSetRequestHeaders()
	{
		// Reset and free any previously set headers
		DoFreeRequestHeaders();

		// Set headers
		for (const WebRequestHeader& header: m_RequestHeaders)
		{
			auto headerString = header.Format().ToUTF8();
			m_RequestHeadersSList = ::curl_slist_append(reinterpret_cast<curl_slist*>(m_RequestHeadersSList), headerString.c_str());
		}
		m_Handle.SetOption(CURLOPT_HTTPHEADER, m_RequestHeadersSList);
	}
	void CURLRequest::DoPerformRequest()
	{
		// Active the request and notify about it
		m_State = WebRequestState::Active;
		NotifyStateChange(WebRequestState::Active);

		// Set error message buffer
		std::array<char, CURL_ERROR_SIZE * 2> errorMessageBuffer;
		errorMessageBuffer.fill(0);
		m_Handle.SetOption(CURLOPT_ERRORBUFFER, errorMessageBuffer.data());

		// Prepare sterams if needed
		if (m_ReceiveStorage == WebRequestStorage::Memory)
		{
			m_ReceiveStream = std::make_shared<MemoryOutputStream>();
		}

		// And start the request
		const int errorCode = ::curl_easy_perform(m_Handle.GetNativeHandle());

		// Get server response code
		const HTTPStatus responseStatus = [&]() -> HTTPStatus
		{
			long responseCode = 0;
			if (::curl_easy_getinfo(m_Handle.GetNativeHandle(), CURLINFO_RESPONSE_CODE, &responseCode) == CURLE_OK)
			{
				return responseCode;
			}
			return {};
		}();
		

		// Decide what to do next
		switch (errorCode)
		{
			case CURLE_OK:
			{
				m_State = WebRequestState::Completed;
				break;
			}
			case CURLE_ABORTED_BY_CALLBACK:
			{
				m_State = WebRequestState::Cancelled;
				NotifyStateChange(WebRequestState::Cancelled, responseStatus, errorMessageBuffer.data());
				break;
			}
			case CURLE_LOGIN_DENIED:
			{
				m_State = WebRequestState::Unauthorized;
				NotifyStateChange(WebRequestState::Unauthorized, responseStatus, errorMessageBuffer.data());
				break;
			}
			default:
			{
				m_State = WebRequestState::Failed;
				NotifyStateChange(WebRequestState::Failed, responseStatus, errorMessageBuffer.data());
				break;
			}
		};
	}
	void CURLRequest::DoCloseRequest() noexcept
	{
		if (m_Handle)
		{
			DoFreeRequestHeaders();

			::curl_easy_cleanup(m_Handle.GetNativeHandle());
			m_Handle = {};
		}
	}

	CURLRequest::CURLRequest(CURLSession& session, const URI& uri)
		:m_Session(session), m_Handle(::curl_easy_init(), CURL::Private::SessionHandleType::Easy)
	{
		static_assert(std::is_same_v<TCURLOffset, curl_off_t>, "'TCURLOffset' and 'curl_off_t' are not the same type");

		// Copy (not move) common headers
		m_RequestHeaders = m_Session.m_CommonHeaders;

		// Upload callback
		m_Handle.SetOption(CURLOPT_READDATA, this);
		m_Handle.SetOption(CURLOPT_READFUNCTION, &CURLRequest::OnReadDataCB);

		// Download callback
		m_Handle.SetOption(CURLOPT_WRITEDATA, this);
		m_Handle.SetOption(CURLOPT_WRITEFUNCTION, &CURLRequest::OnWriteDataCB);

		// Response headers callback
		m_Handle.SetOption(CURLOPT_HEADERDATA, this);
		m_Handle.SetOption(CURLOPT_HEADERFUNCTION, &CURLRequest::OnReceiveHeaderCB);

		// Progress function
		m_Handle.SetOption(CURLOPT_XFERINFODATA, this);
		m_Handle.SetOption(CURLOPT_XFERINFOFUNCTION, &CURLRequest::OnReceiveHeaderCB);
		m_Handle.SetOption(CURLOPT_NOPROGRESS, false);

		// Set default parameters
		CURLRequest::SetURI(uri);
		CURLRequest::SetReceiveStorage(WebRequestStorage::Memory);
	}

	// IWebRequest: Common
	bool CURLRequest::Start()
	{
		if (m_State == WebRequestState::Idle)
		{
			DoSetRequestHeaders();
			return m_Session.StartRequest(*this);
		}
		return false;
	}
	bool CURLRequest::Pause()
	{
		if (m_State == WebRequestState::Active)
		{
			m_NextState = WebRequestState::Paused;
		}
		return false;
	}
	bool CURLRequest::Resume()
	{
		if (m_State == WebRequestState::Paused)
		{
			m_NextState = WebRequestState::Resumed;
			return true;
		}
		return false;
	}
	bool CURLRequest::Cancel()
	{
		if (m_State == WebRequestState::Active || m_State == WebRequestState::Paused)
		{
			m_NextState = WebRequestState::Cancelled;
			return true;
		}
		return false;
	}

	// IWebRequest: Request options
	bool CURLRequest::SetHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags)
	{
		return CURLSession::SetHeader(m_RequestHeaders, header, flags);
	}
	void CURLRequest::ClearHeaders()
	{
		m_RequestHeaders.clear();
	}

	bool CURLRequest::SetSendStorage(WebRequestStorage storage)
	{
		return false;
	}
	bool CURLRequest::SetSendSource(std::shared_ptr<IInputStream> stream)
	{
		if (m_State == WebRequestState::Idle)
		{
			if (m_SendStream = std::move(stream))
			{
				// Reset simple upload data
				m_SendData = {};
				m_SendStorage = WebRequestStorage::Stream;

				// Make sure to use read callback
				m_Handle.SetOption(CURLOPT_POSTFIELDSIZE, 0);
				m_Handle.SetOption(CURLOPT_POSTFIELDS, nullptr);

				// Switch to post request and set upload size
				m_Handle.SetOption(CURLOPT_INFILESIZE_LARGE, m_SendStream->GetSize().ToBytes());
				return m_Handle.SetOption(CURLOPT_POST, true);
			}
		}
		return false;
	}
	bool CURLRequest::SetSendSource(const String& data)
	{
		if (m_State == WebRequestState::Idle)
		{
			auto encodedData = CURL::Private::Escape(m_Handle, data.ToUTF8());
			if (!encodedData.empty() || data.IsEmpty())
			{
				if (CURLRequest::SetSendSource(std::make_shared<MemoryInputStream>(encodedData.data(), encodedData.size())))
				{
					m_SendData = std::move(encodedData);
					m_SendStorage = WebRequestStorage::Memory;

					return true;
				}
			}
		}
		return false;
	}
	bool CURLRequest::SetSendSource(const FSPath& filePath)
	{
		if (m_State == WebRequestState::Idle && filePath)
		{
			auto& fs = m_Session.GetFileSystem();
			if (CURLRequest::SetSendSource(fs.OpenToRead(filePath)))
			{
				m_SendStorage = WebRequestStorage::FileSystem;
				return true;
			}
		}
		return false;
	}

	bool CURLRequest::SetReceiveStorage(WebRequestStorage storage)
	{
		if (m_State == WebRequestState::Idle)
		{
			switch (storage)
			{
				case WebRequestStorage::Memory:
				{
					m_ReceiveStorage = storage;
					return true;
				}
			};
		}
		return false;
	}
	bool CURLRequest::SetReceiveTarget(std::shared_ptr<IOutputStream> stream)
	{
		if (m_State == WebRequestState::Idle)
		{
			if (stream)
			{
				m_ReceiveStream = std::move(stream);
				m_ReceiveStorage = WebRequestStorage::Stream;

				return true;
			}
		}
		return false;
	}
	bool CURLRequest::SetReceiveTarget(const FSPath& filePath)
	{
		if (m_State == WebRequestState::Idle && filePath)
		{
			auto& fs = m_Session.GetFileSystem();
			if (m_ReceiveStream = fs.OpenToWrite(filePath))
			{
				m_ReceiveStorage = WebRequestStorage::FileSystem;
			}
		}
		return false;
	}

	// IWebRequest: Progress
	TransferRate CURLRequest::GetSendRate() const
	{
		curl_off_t rate = -1;
		if (::curl_easy_getinfo(m_Handle.GetNativeHandle(), CURLINFO_SPEED_UPLOAD_T, &rate) == CURLE_OK)
		{
			return TransferRate::FromBytes(rate);
		}
		return {};
	}
	TransferRate CURLRequest::GetReceiveRate() const
	{
		curl_off_t rate = -1;
		if (::curl_easy_getinfo(m_Handle.GetNativeHandle(), CURLINFO_SPEED_DOWNLOAD_T, &rate) == CURLE_OK)
		{
			return TransferRate::FromBytes(rate);
		}
		return {};
	}

	// IWebRequestOptions
	bool CURLRequest::SetURI(const URI& uri)
	{
		if (m_Handle.SetOption(CURLOPT_URL, uri.BuildURI()))
		{
			m_URI = uri;
			return true;
		}
		return false;
	}
	bool CURLRequest::SetPort(uint16_t port)
	{
		return m_Handle.SetOption(CURLOPT_PORT, port);
	}
	bool CURLRequest::SetMethod(const String& method)
	{
		if (!method.IsEmpty())
		{
			return m_Handle.SetOption(CURLOPT_CUSTOMREQUEST, method);
		}
		else
		{
			return m_Handle.SetOption(CURLOPT_CUSTOMREQUEST, nullptr);
		}
	}
	bool CURLRequest::SetDefaultProtocol(const String& protocol)
	{
		return m_Handle.SetOption(CURLOPT_DEFAULT_PROTOCOL, protocol);
	}

	bool CURLRequest::SetServiceName(const String& name)
	{
		return m_Handle.SetOption(CURLOPT_SERVICE_NAME, name);
	}
	bool CURLRequest::SetFollowLocation(WebRequestOption2 option)
	{
		return m_Handle.SetOption(CURLOPT_FOLLOWLOCATION, option == WebRequestOption2::Enabled);
	}
	bool CURLRequest::SetResumeOffset(StreamOffset offset)
	{
		return offset.IsValid() && m_Handle.SetOption(CURLOPT_RESUME_FROM_LARGE, offset.ToBytes());
	}

	bool CURLRequest::SetRequestTimeout(const TimeSpan& timeout)
	{
		return m_Handle.SetOption(CURLOPT_TIMEOUT_MS, timeout.IsPositive() ? timeout.GetMilliseconds() : 0);
	}
	bool CURLRequest::SetConnectionTimeout(const TimeSpan& timeout)
	{
		return m_Handle.SetOption(CURLOPT_CONNECTTIMEOUT_MS, timeout.IsPositive() ? timeout.GetMilliseconds() : 0);
	}

	bool CURLRequest::SetMaxSendRate(const TransferRate& rate)
	{
		return m_Handle.SetOption(CURLOPT_MAX_SEND_SPEED_LARGE, rate.IsValid() ? rate.ToBytes() : 0);
	}
	bool CURLRequest::SetMaxReceiveRate(const TransferRate& rate)
	{
		return m_Handle.SetOption(CURLOPT_MAX_RECV_SPEED_LARGE, rate.IsValid() ? rate.ToBytes() : 0);
	}

	bool CURLRequest::SetKeepAlive(WebRequestOption2 option)
	{
		return m_Handle.SetOption(CURLOPT_TCP_KEEPALIVE, option == WebRequestOption2::Enabled);
	}
	bool CURLRequest::SetKeepAliveIdle(const TimeSpan& interval)
	{
		return m_Handle.SetOption(CURLOPT_TCP_KEEPIDLE, interval.IsPositive() ? interval.GetSeconds() : 60);
	}
	bool CURLRequest::SetKeepAliveInterval(const TimeSpan& interval)
	{
		return m_Handle.SetOption(CURLOPT_TCP_KEEPINTVL, interval.IsPositive() ? interval.GetSeconds() : 60);
	}

	// IWebRequestAuthOptions
	bool CURLRequest::SetAuthMethod(WebRequestAuthMethod method)
	{
		switch (method)
		{
			case WebRequestAuthMethod::Basic:
			{
				return m_Handle.SetOption(CURLOPT_HTTPAUTH, CURLAUTH_ONLY|CURLAUTH_BASIC);
			}
			case WebRequestAuthMethod::NTLM:
			{
				return m_Handle.SetOption(CURLOPT_HTTPAUTH, CURLAUTH_ONLY|CURLAUTH_NTLM);
			}
			case WebRequestAuthMethod::Digest:
			{
				return m_Handle.SetOption(CURLOPT_HTTPAUTH, CURLAUTH_ONLY|CURLAUTH_DIGEST);
			}
			case WebRequestAuthMethod::Bearer:
			{
				return m_Handle.SetOption(CURLOPT_HTTPAUTH, CURLAUTH_ONLY|CURLAUTH_BEARER);
			}
			case WebRequestAuthMethod::Negotiate:
			{
				return m_Handle.SetOption(CURLOPT_HTTPAUTH, CURLAUTH_ONLY|CURLAUTH_NEGOTIATE);
			}
		};
		return false;
	}
	bool CURLRequest::SetAuthMethods(FlagSet<WebRequestAuthMethod> methods)
	{
		FlagSet<uint32_t> curlFlags;
		curlFlags.Add(CURLAUTH_BASIC, methods & WebRequestAuthMethod::Basic);
		curlFlags.Add(CURLAUTH_NTLM, methods & WebRequestAuthMethod::NTLM);
		curlFlags.Add(CURLAUTH_DIGEST, methods & WebRequestAuthMethod::Digest);
		curlFlags.Add(CURLAUTH_BEARER, methods & WebRequestAuthMethod::Bearer);
		curlFlags.Add(CURLAUTH_NEGOTIATE, methods & WebRequestAuthMethod::Negotiate);

		return m_Handle.SetOption(CURLOPT_HTTPAUTH, *curlFlags);
	}

	bool CURLRequest::SetUserName(const String& userName)
	{
		return m_Handle.SetOption(CURLOPT_USERNAME, userName);
	}
	bool CURLRequest::SetUserPassword(const String& userPassword)
	{
		return m_Handle.SetOption(CURLOPT_PASSWORD, userPassword);
	}

	// IWebRequestSecurityOptions
	bool CURLRequest::SetUseSSL(WebRequestOption3 option)
	{
		switch (option)
		{
			case WebRequestOption3::Diabled:
			{
				return m_Handle.SetOption(CURLOPT_USE_SSL, CURLUSESSL_NONE);
			}
			case WebRequestOption3::Enabled:
			{
				return m_Handle.SetOption(CURLOPT_USE_SSL, CURLUSESSL_TRY);
			}
			case WebRequestOption3::Required:
			{
				return m_Handle.SetOption(CURLOPT_USE_SSL, CURLUSESSL_ALL);
			}
		};
		return false;
	}
	bool CURLRequest::SetSSLVersion(WebRequestSSLVersion version)
	{
		switch (version)
		{
			case WebRequestSSLVersion::Default:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
			}
			case WebRequestSSLVersion::SSLv2:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv2);
			}
			case WebRequestSSLVersion::SSLv3:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
			}
			case WebRequestSSLVersion::TLSv1_x:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1);
			}
			case WebRequestSSLVersion::TLSv1_0:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_0);
			}
			case WebRequestSSLVersion::TLSv1_1:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_1);
			}
			case WebRequestSSLVersion::TLSv1_2:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
			}
			case WebRequestSSLVersion::TLSv1_3:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);
			}
		};
		return false;
	}
	bool CURLRequest::SetMaxSSLVersion(WebRequestSSLVersion version)
	{
		switch (version)
		{
			case WebRequestSSLVersion::Default:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_DEFAULT);
			}
			case WebRequestSSLVersion::TLSv1_0:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_0);
			}
			case WebRequestSSLVersion::TLSv1_1:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_1);
			}
			case WebRequestSSLVersion::TLSv1_2:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_2);
			}
			case WebRequestSSLVersion::TLSv1_3:
			{
				return m_Handle.SetOption(CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_3);
			}
		};
		return false;
	}

	bool CURLRequest::SetVerifyPeer(WebRequestOption2 option)
	{
		return m_Handle.SetOption(CURLOPT_SSL_VERIFYPEER, option == WebRequestOption2::Enabled);
	}
	bool CURLRequest::SetVerifyHost(WebRequestOption2 option)
	{
		return m_Handle.SetOption(CURLOPT_SSL_VERIFYHOST, option == WebRequestOption2::Enabled ? 2 : 0);
	}
	bool CURLRequest::SetVerifyStatus(WebRequestOption2 option)
	{
		return m_Handle.SetOption(CURLOPT_SSL_VERIFYSTATUS, option == WebRequestOption2::Enabled);
	}

	Enumerator<String> CURLRequest::EnumReplyCookies() const
	{
		curl_slist* cookesList = nullptr;
		const CURLcode result = ::curl_easy_getinfo(m_Handle.GetNativeHandle(), CURLINFO_COOKIELIST, &cookesList);
		if (result == CURLE_OK && cookesList)
		{
			return [handle = make_handle_ptr<::curl_slist_free_all>(cookesList), item = cookesList]() mutable -> std::optional<String>
			{
				auto curentItem = item;
				item = item->next;

				if (curentItem)
				{
					return String::FromUTF8(curentItem->data);
				}
				return {};
			};
		}
		return {};
	}
}
