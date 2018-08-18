#include "KxStdAfx.h"
#include "KxFramework/KxINet.h"
#include "KxFramework/KxINetConnection.h"
#include "KxFramework/KxINetEvent.h"
#include <WinINet.h>
#include <WinDNS.h>
#pragma comment(lib, "DnsAPI.lib")
#pragma comment(lib, "WinINet.lib")

const uint16_t KxINet::DefaultPort = INTERNET_INVALID_PORT_NUMBER;
const uint16_t KxINet::DefaultPortHTTP = INTERNET_DEFAULT_HTTP_PORT;
const uint16_t KxINet::DefaultPortHTTPS = INTERNET_DEFAULT_HTTPS_PORT;

KxINetURLParts KxINet::SplitURL(const wxString& url)
{
	URL_COMPONENTSW urlParts = {0};
	urlParts.dwStructSize = sizeof(URL_COMPONENTSW);
	urlParts.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
	urlParts.dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;
	urlParts.dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;
	urlParts.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
	urlParts.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
	urlParts.dwExtraInfoLength = INTERNET_MAX_PATH_LENGTH;

	KxINetURLParts splitData;

	wxString urlCopy = KxString::ToLower(url);
	urlCopy.Replace(" ", wxEmptyString, true);
	if (::InternetCrackUrlW(urlCopy.wc_str(), urlCopy.Length(), NULL, &urlParts))
	{
		// Port
		splitData.Port = urlParts.nPort;

		// Scheme
		switch (urlParts.nScheme)
		{
			case INTERNET_SCHEME_HTTP:
			case INTERNET_SCHEME_DEFAULT:
			{
				splitData.ShemeType = KxINET_SCHEME_HTTP;
				break;
			}
			case INTERNET_SCHEME_HTTPS:
			{
				splitData.ShemeType = KxINET_SCHEME_HTTPS;
				break;
			}
			case INTERNET_SCHEME_FTP:
			{
				splitData.ShemeType = KxINET_SCHEME_FTP;
				break;
			}
		};

		auto ExtractField = [](DWORD size, const auto& oldField, auto& newField)
		{
			if (oldField)
			{
				newField = wxString(oldField, size);
			}
		};
		ExtractField(urlParts.dwHostNameLength, urlParts.lpszHostName, splitData.HostName);
		ExtractField(urlParts.dwSchemeLength, urlParts.lpszScheme, splitData.Sheme);
		ExtractField(urlParts.dwUrlPathLength, urlParts.lpszUrlPath, splitData.Path);
		ExtractField(urlParts.dwExtraInfoLength, urlParts.lpszExtraInfo, splitData.ExtraInfo);
		ExtractField(urlParts.dwUserNameLength, urlParts.lpszUserName, splitData.UserName);
		ExtractField(urlParts.dwPasswordLength, urlParts.lpszPassword, splitData.Password);

		// File name
		if (!splitData.Path.IsEmpty())
		{
			splitData.FileName = splitData.Path.AfterLast(L'/');
			if (splitData.FileName == splitData.Path)
			{
				splitData.FileName = wxEmptyString;
			}
		}

		// Full path
		splitData.FullPath = splitData.HostName + splitData.Path + splitData.ExtraInfo;
		if (splitData.FullPath.IsEmpty())
		{
			splitData.FullPath = urlCopy;
		}

		if (splitData.ShemeType == KxINET_SCHEME_UNKNOWN)
		{
			if (splitData.Sheme == "ws")
			{
				splitData.ShemeType = KxINET_SCHEME_HTTP;
			}
			else if (splitData.Sheme == "wss")
			{
				splitData.ShemeType = KxINET_SCHEME_HTTPS;
			}
		}
	}

	return splitData;
}
bool KxINet::IsInternetAvailable()
{
	DWORD flags = 0;
	return ::InternetGetConnectedState(&flags, 0);
}
wxString KxINet::LookupIP(const wxString& url)
{
	KxINetURLParts parts = SplitURL(url);

	PDNS_RECORD infoDNS = NULL;
	if (::DnsQuery_W(parts.HostName, DNS_TYPE_A, DNS_QUERY_STANDARD|DNS_QUERY_BYPASS_CACHE, NULL, &infoDNS, NULL) == 0)
	{
		IN_ADDR inAddress;
		inAddress.S_un.S_addr = (infoDNS->Data.A.IpAddress);
		DnsRecordListFree(infoDNS, DnsFreeRecordListDeep);

		return inet_ntoa(inAddress);
	}
	return wxEmptyString;
}

//////////////////////////////////////////////////////////////////////////
void KxINet::SetTimeouts()
{
	::InternetSetOptionW(m_Handle, INTERNET_OPTION_CONNECT_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	::InternetSetOptionW(m_Handle, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	::InternetSetOptionW(m_Handle, INTERNET_OPTION_RECEIVE_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	::InternetSetOptionW(m_Handle, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	::InternetSetOptionW(m_Handle, INTERNET_OPTION_DATA_SEND_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	::InternetSetOptionW(m_Handle, INTERNET_OPTION_FROM_CACHE_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	::InternetSetOptionW(m_Handle, INTERNET_OPTION_SEND_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	//::InternetSetOptionW(m_Handle, INTERNET_OPTION_LISTEN_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
	//::InternetSetOptionW(m_Handle, INTERNET_OPTION_DISCONNECTED_TIMEOUT, &m_TimeOut, sizeof(m_TimeOut));
}

KxINetConnection* KxINet::OnMakeConnection(const wxString& url, uint16_t port, const wxString& userName, const wxString& password)
{
	return new KxINetConnection(this, url, port, userName, password);
}
wxString KxINet::OnGetUserAgent() const
{
	return "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36";
}

KxINet::KxINet(DWORD timeout)
	:m_TimeOut(timeout)
{
	wxString sUserAgent = OnGetUserAgent();
	m_Handle = ::InternetOpenW(sUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	SetTimeouts();
}
KxINet::~KxINet()
{
	::InternetCloseHandle(m_Handle);
}

void KxINet::DestroyConnection(KxINetConnection& connection)
{
	m_ConnectionsList.remove_if([&connection](const auto& p)
	{
		return p.get() == &connection;
	});
}
KxINetConnection& KxINet::MakeConnection(const wxString& url, uint16_t port, const wxString& userName, const wxString& password)
{
	KxINetConnection* pConnection = m_ConnectionsList.emplace_back(OnMakeConnection(url, port, userName, password)).get();
	return *pConnection;
}

const WCHAR* KxINet::AcceptTypes[] =
{
	L"application/*",
	L"audio/*",
	L"example/*",
	L"image/*",
	L"message/*",
	L"model/*",
	L"multipart/*",
	L"text/*",
	L"video/*",
	NULL
};