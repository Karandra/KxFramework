#include "KxStdAfx.h"
#include "KxFramework/KxCURL.h"

#define CURL_STATICLIB 1
#include "KxFramework/cURL/curl.h"
#include "KxFramework/cURL/urlapi.h"
#include "KxFramework/cURL/curlver.h"

#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "Normaliz.lib")
#if defined _WIN64
#pragma comment(lib, "KxFramework/cURL/libcurl x64.lib")
#else
#pragma comment(lib, "KxFramework/cURL/libcurl x86.lib")
#endif

namespace cURL
{
	template<class HandleT, class OptionT, class T, bool easy> int SetOptionInt(HandleT handle, OptionT option, T value)
	{
		using ValueT = typename std::remove_cv<T>::type;

		if constexpr(std::is_pointer<ValueT>::value)
		{
			if constexpr(easy)
			{
				return curl_easy_setopt(handle, static_cast<CURLoption>(option), (void*)value);
			}
			else
			{
				return curl_multi_setopt(handle, static_cast<CURLMoption>(option), (void*)value);
			}
		}
		else if constexpr(sizeof(ValueT) <= sizeof(long))
		{
			if constexpr(easy)
			{
				return curl_easy_setopt(handle, static_cast<CURLoption>(option), static_cast<long>(value));
			}
			else
			{
				return curl_multi_setopt(handle, static_cast<CURLMoption>(option), static_cast<long>(value));
			}
		}
		else if constexpr(sizeof(ValueT) <= sizeof(curl_off_t))
		{
			if constexpr(easy)
			{
				return curl_easy_setopt(handle, static_cast<CURLoption>(option), static_cast<curl_off_t>(value));
			}
			else
			{
				return curl_multi_setopt(handle, static_cast<CURLMoption>(option), static_cast<curl_off_t>(value));
			}
		}
		else
		{
			static_assert(false, "cURL::SetOptionInt: Unknown type");
			if constexpr(easy)
			{
				return CURL_LAST;
			}
			else
			{
				return CURLM_LAST;
			}
		}
	}
	
	template<class T> int SetOptionIntEasy(CURL* handle, CURLoption option, T value)
	{
		return SetOptionInt<CURL*, CURLoption, T, true>(handle, option, value);
	}
	template<class T> int SetOptionIntMulti(CURLM* handle, CURLMoption option, T value)
	{
		return SetOptionInt<CURLM*, CURLMoption, T, false>(handle, option, value);
	}
}

//////////////////////////////////////////////////////////////////////////
wxString KxCURL::GetLibraryName()
{
	return wxS("libcurl");
}
wxString KxCURL::GetLibraryVersion()
{
	return LIBCURL_VERSION;
}

KxCURL::KxCURL()
{
}
KxCURL::~KxCURL()
{	
}

wxString KxCURL::ErrorCodeToString(int code) const
{
	return wxString::FromUTF8(curl_easy_strerror(static_cast<CURLcode>(code)));
}

//////////////////////////////////////////////////////////////////////////
size_t KxCURLSession::OnWriteResponse(char* data, size_t size, size_t count, void* userData)
{
	CallbackData* callbackData = reinterpret_cast<CallbackData*>(userData);
	KxCURLSession& session = callbackData->GetSession();
	KxCURLReplyBase& reply = callbackData->GetReply();

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
		const size_t length = size * count;
		reply.AddChunk(data, length);

		// Send event
		KxCURLEvent event(KxEVT_CURL_DOWNLOAD, &session, &reply);
		event.SetEventObject(&session);
		event.SetSource(session.m_URL.BuildUnescapedURI());
		event.SetMajorProcessed(reply.GetDownloaded());
		event.SetResponseData(data, length);

		// Get total size
		curl_off_t contentLength = -1;
		curl_easy_getinfo(session.GetHandle(), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &contentLength);
		event.SetMajorTotal(contentLength);

		// Get average speed
		curl_off_t downloadSpeed = -1;
		curl_easy_getinfo(session.GetHandle(), CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed);
		event.SetSpeed(downloadSpeed);

		session.SafelyProcessEvent(event);
		return length;
	}
}
size_t KxCURLSession::OnWriteHeader(char* data, size_t size, size_t count, void* userData)
{
	CallbackData* callbackData = reinterpret_cast<CallbackData*>(userData);
	KxCURLSession& session = callbackData->GetSession();
	KxCURLReplyBase& reply = callbackData->GetReply();

	// Save header
	const size_t length = size * count;

	// Send event
	KxCURLEvent event(KxEVT_CURL_RESPONSE_HEADER, &session, &reply);
	event.SetEventObject(&session);
	event.SetSource(session.m_URL.BuildUnescapedURI());
	event.SetResponseData(data, length);
	session.SafelyProcessEvent(event);

	return length;
}

int KxCURLSession::SetOption(int option, const wxString& value, size_t* length)
{
	auto utf8 = value.ToUTF8();
	if (length)
	{
		*length = utf8.length();
	}
	return cURL::SetOptionIntEasy(m_Handle, static_cast<CURLoption>(option), utf8.data());
}
int KxCURLSession::SetOption(int option, int value)
{
	return cURL::SetOptionIntEasy(m_Handle, static_cast<CURLoption>(option), value);
}
int KxCURLSession::SetOption(int option, size_t value)
{
	return cURL::SetOptionIntEasy(m_Handle, static_cast<CURLoption>(option), value);
}
int KxCURLSession::SetOption(int option, int64_t value)
{
	return cURL::SetOptionIntEasy(m_Handle, static_cast<CURLoption>(option), value);
}
int KxCURLSession::SetOption(int option, bool value)
{
	return cURL::SetOptionIntEasy(m_Handle, static_cast<CURLoption>(option), value);
}
int KxCURLSession::SetOption(int option, const void* value)
{
	return cURL::SetOptionIntEasy(m_Handle, static_cast<CURLoption>(option), value);
}

void KxCURLSession::SetHeaders()
{
	if (m_HeadersSList)
	{
		curl_slist_free_all(reinterpret_cast<curl_slist*>(m_HeadersSList));
		m_HeadersSList = nullptr;
	}
	for (const std::string& header: m_Headers)
	{
		m_HeadersSList = curl_slist_append(reinterpret_cast<curl_slist*>(m_HeadersSList), header.c_str());
	}
	curl_easy_setopt(m_Handle, CURLOPT_HTTPHEADER, m_HeadersSList);
}
void KxCURLSession::DoSendRequest(KxCURLReplyBase& reply)
{
	m_IsStopped = false;
	m_IsPaused = false;

	SetHeaders();

	SetOption(CURLOPT_URL, m_URL.BuildURI());
	SetOption(CURLOPT_FOLLOWLOCATION, true);
	SetOption(CURLOPT_SSL_VERIFYPEER, false);

	// Set user agent if specified
	if (!m_UserAgent.IsEmpty())
	{
		SetOption(CURLOPT_USERAGENT, m_UserAgent);
	}

	// Post data
	if (!m_PostData.IsEmpty())
	{
		size_t length = 0;
		SetOption(CURLOPT_POSTFIELDS, m_PostData, &length);
		SetOption(CURLOPT_POSTFIELDSIZE_LARGE, length);
	}

	// Server response
	CallbackData callbackData(*this, reply);

	// On data
	SetOption(CURLOPT_WRITEDATA, &callbackData);
	SetOptionFunction(CURLOPT_WRITEFUNCTION, OnWriteResponse);

	// On headers
	SetOption(CURLOPT_HEADERDATA, &callbackData);
	SetOptionFunction(CURLOPT_HEADERFUNCTION, OnWriteHeader);

	// Set error message buffer
	std::array<char, 2 * CURL_ERROR_SIZE> errorMessage;
	errorMessage.fill(0);
	SetOption(CURLOPT_ERRORBUFFER, errorMessage.data());

	// Send
	reply.SetErrorCode(curl_easy_perform(m_Handle));
	reply.SetErrorMessage(wxString::FromUTF8(errorMessage.data()));

	// Get server response code
	long responseCode = 0;
	curl_easy_getinfo(m_Handle, CURLINFO_RESPONSE_CODE, &responseCode);
	if (responseCode != 0)
	{
		reply.SetResponseCode(responseCode);
	}
}

KxCURLSession::KxCURLSession(const KxURL& url)
{
	m_Handle = curl_easy_init();
	SetURL(url);
}
KxCURLSession::~KxCURLSession()
{
	Close();
}

void KxCURLSession::Close()
{
	if (m_HeadersSList)
	{
		curl_slist_free_all(reinterpret_cast<curl_slist*>(m_HeadersSList));
		m_HeadersSList = nullptr;
	}

	if (m_Handle)
	{
		curl_easy_cleanup(m_Handle);
		m_Handle = nullptr;
	}
}
void KxCURLSession::Download(KxCURLStreamReply& reply)
{
	int64_t initialPos = 0;
	if (reply.ShouldResumeFromPosition(initialPos))
	{
		SetOption(CURLOPT_RESUME_FROM_LARGE, initialPos);
	}
	DoSendRequest(reply);
}

KxStringVector KxCURLSession::GetReplyCookies() const
{
	KxStringVector cookies;

	curl_slist* cookesList = nullptr;
	CURLcode res = curl_easy_getinfo(m_Handle, CURLINFO_COOKIELIST, &cookesList);
	if (res == CURLE_OK && cookesList)
	{
		const curl_slist* item = cookesList;
		while (item)
		{
			cookies.push_back(wxString::FromUTF8(item->data));
			item = item->next;
		}
		curl_slist_free_all(cookesList);
	}
	return cookies;
}

bool KxCURLSession::Pause()
{
	if (curl_easy_pause(m_Handle, CURLPAUSE_ALL) == CURLE_OK)
	{
		m_IsPaused = true;
		return true;
	}
	return false;
}
bool KxCURLSession::Resume()
{
	if (curl_easy_pause(m_Handle, CURLPAUSE_CONT) == CURLE_OK)
	{
		m_IsPaused = false;
		return true;
	}
	return false;
}
void KxCURLSession::Stop()
{
	m_IsStopped = true;
}

void KxCURLSession::SetURL(const KxURL& url)
{
	m_URL = url;
}
void KxCURLSession::SetPostData(const wxString& data)
{
	m_PostData = data;
}

void KxCURLSession::AddHeader(const wxString& name, const wxString& value)
{
	m_Headers.push_back(wxString::Format(wxS("%s: %s"), name, value).ToStdString());
}
void KxCURLSession::AddHeader(const wxString& value)
{
	m_Headers.push_back(value.ToStdString());
}

void KxCURLSession::SetTimeout(const wxTimeSpan& timeout)
{
	SetOption(CURLOPT_TIMEOUT_MS, timeout.GetMilliseconds().GetValue());
}
void KxCURLSession::SetConnectionTimeout(const wxTimeSpan& timeout)
{
	SetOption(CURLOPT_CONNECTTIMEOUT_MS, timeout.GetMilliseconds().GetValue());
}

KxCURLSession& KxCURLSession::operator=(KxCURLSession&& other)
{
	if (this != &other)
	{
		Close();

		m_Handle = other.m_Handle;
		m_IsPaused = other.m_IsPaused;
		m_IsStopped = other.m_IsStopped;

		m_HeadersSList = other.m_HeadersSList;
		m_Headers = std::move(other.m_Headers);

		m_URL = std::move(other.m_URL);
		m_PostData = other.m_PostData;
		m_UserAgent = other.m_UserAgent;

		other.m_Handle = nullptr;
		other.m_IsPaused = false;
		other.m_IsStopped = false;

		other.m_HeadersSList = nullptr;

		other.m_PostData.clear();
		other.m_UserAgent.clear();
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
class KxCURLModule: public wxModule
{
	public:
		bool OnInit() override
		{
			const CURLcode status = curl_global_init(CURL_GLOBAL_DEFAULT);
			KxCURL::GetInstance().m_IsInitialized = status == CURLE_OK;

			return true;
		}
		void OnExit() override
		{
			curl_global_cleanup();
			KxCURL::GetInstance().m_IsInitialized = false;
		}

	private:
		wxDECLARE_DYNAMIC_CLASS(KxCURLModule);
};
wxIMPLEMENT_DYNAMIC_CLASS(KxCURLModule, wxModule);
