#include "KxStdAfx.h"
#include "Session.h"
#include "Kx/General/StringFormater.h"
#include "Kx/Utility/CallAtScopeExit.h"

#define CURL_STATICLIB 1
#include <curl/curl.h>
#include <curl/urlapi.h>
#include <curl/curlver.h>

namespace KxFramework
{
	size_t CURLSession::OnWriteResponse(char* data, size_t size, size_t count, void* userData)
	{
		auto callbackData = reinterpret_cast<CURL::Private::CallbackData*>(userData);
		CURLSession& session = callbackData->GetSession();
		CURLReplyBase& reply = callbackData->GetReply();

		if (session.IsStopped())
		{
			return 0;
		}
		else if (session.IsPaused())
		{
			return CURL_WRITEFUNC_PAUSE;
		}
		else
		{
			// Write data
			const size_t dataLength = size * count;
			reply.AddProcessedData(data, dataLength);

			// Send event
			CURLEvent event(CURLEvent::EvtDownload, &session, &reply);
			event.SetEventObject(&session);
			event.SetSource(session.m_URI.BuildUnescapedURI());
			event.SetProcessed(BinarySize::FromBytes(reply.GetProcessed()));
			event.SetResponseData(data, dataLength);

			// Get total size
			curl_off_t contentLength = -1;
			curl_easy_getinfo(session.GetHandle().GetNativeHandle(), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &contentLength);
			event.SetTotal(BinarySize::FromBytes(contentLength));

			// Get average speed
			curl_off_t downloadSpeed = -1;
			curl_easy_getinfo(session.GetHandle().GetNativeHandle(), CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed);
			event.SetSpeed(BinarySize::FromBytes(downloadSpeed));

			session.SafelyProcessEvent(event);
			return dataLength;
		}
	}
	size_t CURLSession::OnWriteHeader(char* data, size_t size, size_t count, void* userData)
	{
		auto* callbackData = reinterpret_cast<CURL::Private::CallbackData*>(userData);
		CURLSession& session = callbackData->GetSession();
		CURLReplyBase& reply = callbackData->GetReply();

		// Save header
		const size_t length = size * count;

		// Send event
		CURLEvent event(CURLEvent::EvtResponseHeader, &session, &reply);
		event.SetEventObject(&session);
		event.SetSource(session.m_URI.BuildUnescapedURI());
		event.SetResponseData(data, length);
		session.SafelyProcessEvent(event);

		return length;
	}

	void CURLSession::SetHeaders()
	{
		if (m_HeadersSList)
		{
			curl_slist_free_all(reinterpret_cast<curl_slist*>(m_HeadersSList));
			m_HeadersSList = nullptr;
		}
		for (const std::string& header: m_SessionHeaders)
		{
			m_HeadersSList = curl_slist_append(reinterpret_cast<curl_slist*>(m_HeadersSList), header.c_str());
		}
		curl_easy_setopt(m_Handle.GetNativeHandle(), CURLOPT_HTTPHEADER, m_HeadersSList);
	}
	void CURLSession::DoSendRequest(CURLReplyBase& reply)
	{
		m_IsStopped = false;
		m_IsPaused = false;

		SetHeaders();

		m_Handle.SetOption(CURLOPT_URL, m_URI.BuildURI());
		m_Handle.SetOption(CURLOPT_FOLLOWLOCATION, true);
		m_Handle.SetOption(CURLOPT_SSL_VERIFYPEER, false);

		// Set user agent if specified
		if (!m_UserAgent.IsEmpty())
		{
			m_Handle.SetOption(CURLOPT_USERAGENT, m_UserAgent);
		}

		// Post data
		if (!m_PostData.IsEmpty())
		{
			size_t length = 0;
			m_Handle.SetOption(CURLOPT_POSTFIELDS, m_PostData, &length);
			m_Handle.SetOption(CURLOPT_POSTFIELDSIZE_LARGE, length);
		}

		// Server response
		CURL::Private::CallbackData callbackData(*this, reply);

		// On data
		m_Handle.SetOption(CURLOPT_WRITEDATA, &callbackData);
		m_Handle.SetOption(CURLOPT_WRITEFUNCTION, OnWriteResponse);

		// On headers
		m_Handle.SetOption(CURLOPT_HEADERDATA, &callbackData);
		m_Handle.SetOption(CURLOPT_HEADERFUNCTION, OnWriteHeader);

		// Set error message buffer
		std::array<char, 2 * CURL_ERROR_SIZE> errorMessage;
		errorMessage.fill(0);
		m_Handle.SetOption(CURLOPT_ERRORBUFFER, errorMessage.data());

		// Send
		reply.SetErrorCode(curl_easy_perform(m_Handle.GetNativeHandle()));
		reply.SetErrorMessage(String::FromUTF8(errorMessage.data()));

		// Get server response code
		long responseCode = 0;
		curl_easy_getinfo(m_Handle.GetNativeHandle(), CURLINFO_RESPONSE_CODE, &responseCode);
		if (responseCode != 0)
		{
			reply.SetResponseCode(responseCode);
		}
	}

	CURLSession::CURLSession(const URI& url)
		:m_Handle(curl_easy_init(), CURL::Private::SessionHandleType::Easy)
	{
		SetURI(url);
	}

	void CURLSession::Close() noexcept
	{
		if (m_HeadersSList)
		{
			curl_slist_free_all(reinterpret_cast<curl_slist*>(m_HeadersSList));
			m_HeadersSList = nullptr;
		}
		if (m_Handle)
		{
			curl_easy_cleanup(m_Handle.GetNativeHandle());
			m_Handle = {};
		}
	}
	void CURLSession::Download(CURLStreamReply& reply)
	{
		int64_t initialPos = 0;
		if (reply.ShouldResumeFromPosition(initialPos))
		{
			m_Handle.SetOption(CURLOPT_RESUME_FROM_LARGE, initialPos);
		}
		DoSendRequest(reply);
	}

	size_t CURLSession::EnumReplyCookies(std::function<bool(String)> func) const
	{
		curl_slist* cookesList = nullptr;
		Utility::CallAtScopeExit atExit = [&]()
		{
			if (cookesList)
			{
				curl_slist_free_all(cookesList);
			}
		};

		const CURLcode result = curl_easy_getinfo(m_Handle.GetNativeHandle(), CURLINFO_COOKIELIST, &cookesList);
		if (result == CURLE_OK && cookesList)
		{
			const curl_slist* item = cookesList;

			size_t count = 0;
			while (item)
			{
				count++;
				if (!std::invoke(func, String::FromUTF8(item->data)))
				{
					break;
				}
				item = item->next;
			}
			return count;
		}
		return 0;
	}

	bool CURLSession::Pause() noexcept
	{
		if (curl_easy_pause(m_Handle.GetNativeHandle(), CURLPAUSE_ALL) == CURLE_OK)
		{
			m_IsPaused = true;
			return true;
		}
		return false;
	}
	bool CURLSession::Resume() noexcept
	{
		if (curl_easy_pause(m_Handle.GetNativeHandle(), CURLPAUSE_CONT) == CURLE_OK)
		{
			m_IsPaused = false;
			return true;
		}
		return false;
	}
	void CURLSession::Stop() noexcept
	{
		m_IsStopped = true;
	}

	void CURLSession::SetURI(const URI& url)
	{
		m_URI = url;
	}
	void CURLSession::SetPostData(const String& data)
	{
		m_PostData = data;
	}

	void CURLSession::AddHeader(const String& name, const String& value)
	{
		m_SessionHeaders.emplace_back(String::Format(wxS("%s: %s"), name, value).ToStdString());
	}
	void CURLSession::AddHeader(const String& value)
	{
		m_SessionHeaders.emplace_back(value.ToStdString());
	}

	void CURLSession::SetTimeout(const wxTimeSpan& timeout) noexcept
	{
		m_Handle.SetOption(CURLOPT_TIMEOUT_MS, timeout.GetMilliseconds().GetValue());
	}
	void CURLSession::SetConnectionTimeout(const wxTimeSpan& timeout) noexcept
	{
		m_Handle.SetOption(CURLOPT_CONNECTTIMEOUT_MS, timeout.GetMilliseconds().GetValue());
	}

	CURLSession& CURLSession::operator=(CURLSession&& other) noexcept
	{
		if (this != &other)
		{
			Close();

			m_Handle = std::move(other.m_Handle);
			other.m_Handle = {};

			m_IsPaused = other.m_IsPaused;
			other.m_IsPaused = false;

			m_IsStopped = other.m_IsStopped;
			other.m_IsStopped = false;

			m_HeadersSList = other.m_HeadersSList;
			other.m_HeadersSList = nullptr;

			m_SessionHeaders = std::move(other.m_SessionHeaders);
			m_URI = std::move(other.m_URI);
			m_PostData = std::move(other.m_PostData);
			m_UserAgent = std::move(other.m_UserAgent);
		}
		return *this;
	}
}
