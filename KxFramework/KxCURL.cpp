#include "KxStdAfx.h"
#include "KxFramework/KxCURL.h"

#define CURL_STATICLIB 1
#include "KxFramework/cURL/curl.h"
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
				return curl_easy_setopt(handle, static_cast<CURLoption>(option), static_cast<curl_off_t >(value));
			}
			else
			{
				return curl_multi_setopt(handle, static_cast<CURLMoption>(option), static_cast<curl_off_t >(value));
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
wxString KxCURL::GetVersion()
{
	return LIBCURL_VERSION;
}

KxCURL::KxCURL()
{
}
KxCURL::~KxCURL()
{	
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
		reply.AddChunk(data, size * count);

		// Send event
		KxCURLEvent event(KxEVT_CURL_DOWNLOAD, &session, &reply);
		event.SetEventObject(&session);
		event.SetSource(session.m_URL);
		event.SetMajorProcessed(reply.GetDownloaded());

		// Get total size
		curl_off_t contentLength = -1;
		curl_easy_getinfo(session.GetHandle(), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &contentLength);
		event.SetMajorTotal(contentLength);

		// Get average speed
		curl_off_t downloadSpeed = -1;
		curl_easy_getinfo(session.GetHandle(), CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed);
		event.SetSpeed(downloadSpeed);

		session.SafelyProcessEvent(event);
		return size * count;
	}
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
	SetOption(CURLOPT_URL, m_URL);
	SetOption(CURLOPT_FOLLOWLOCATION, true);
	SetOption(CURLOPT_SSL_VERIFYPEER, false);

	// Post data
	if (!m_PostData.IsEmpty())
	{
		size_t length = 0;
		SetOption(CURLOPT_POSTFIELDS, m_PostData, &length);
		SetOption(CURLOPT_POSTFIELDSIZE_LARGE, length);
	}

	// Server response
	CallbackData callbackData(*this, reply);
	SetOption(CURLOPT_WRITEDATA, &callbackData);
	SetOptionFunction(CURLOPT_WRITEFUNCTION, OnWriteResponse);

	// Send
	reply.SetErrorCode(curl_easy_perform(m_Handle));

	// Get server response code
	long responseCode = 0;
	curl_easy_getinfo(m_Handle, CURLINFO_RESPONSE_CODE, &responseCode);
	if (responseCode != 0)
	{
		reply.SetResponseCode(responseCode);
	}
}

KxCURLSession::KxCURLSession(const wxString& url)
	:m_URL(url)
{
	m_Handle = curl_easy_init();
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
		m_HeadersSList = NULL;
	}

	if (m_Handle)
	{
		curl_easy_cleanup(m_Handle);
		m_Handle = NULL;
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

	curl_slist* cookesList = NULL;
	CURLcode res = curl_easy_getinfo(m_Handle, CURLINFO_COOKIELIST, &cookesList);
	if (res == CURLE_OK && cookesList)
	{
		curl_slist* each = cookesList;
		while (each)
		{
			cookies.push_back(wxString::FromUTF8(each->data));
			each = each->next;
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

void KxCURLSession::AddHeader(const wxString& name, const wxString& value)
{
	m_Headers.push_back(wxString::Format("%s: %s", name, value).ToStdString());
}
void KxCURLSession::AddHeader(const wxString& value)
{
	m_Headers.push_back(value.ToStdString());
}

//////////////////////////////////////////////////////////////////////////
class KxCURLModule: public wxModule
{
	public:
		virtual bool OnInit() override
		{
			CURLcode status = curl_global_init(CURL_GLOBAL_DEFAULT);
			KxCURL::GetInstance().m_IsInitialized = status == CURLE_OK;
			return true;
		}
		virtual void OnExit() override
		{
			curl_global_cleanup();
		}

	private:
		wxDECLARE_DYNAMIC_CLASS(KxCURLModule);
};
wxIMPLEMENT_DYNAMIC_CLASS(KxCURLModule, wxModule);
