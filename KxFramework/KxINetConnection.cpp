#include "KxStdAfx.h"
#include "KxFramework/KxINet.h"
#include "KxFramework/KxINetConnection.h"
#include "KxFramework/KxINetRequest.h"
#include "KxFramework/KxINetEvent.h"
#include <WinINet.h>

void KxINetConnection::AddRequestHeadres(const KxINetRequest& request, HINTERNET requestHandle)
{
	for (const auto& header: request.GetHeaders())
	{
		wxString headerString = wxString::Format("%s: %s\r\n", header.first, header.second);
		bool headerAdded = false;

		if (headerString.IsAscii())
		{
			headerAdded = ::HttpAddRequestHeadersA(requestHandle, headerString.c_str(), (DWORD)-1, HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA);
		}
		else
		{
			headerAdded = ::HttpAddRequestHeadersW(requestHandle, headerString.wc_str(), (DWORD)-1, HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA);
		}

		if (!headerAdded)
		{
			wxLogError("Header not added: '%s: %s'", header.first, header.second);
		}
	}
}
bool KxINetConnection::SendQueryEvent(HINTERNET requestHandle)
{
	KxINetEvent event(KxEVT_INET_QUERY_INFO, this);
	event.SetRequestHandle(requestHandle);
	ProcessEvent(event);

	return event.IsAllowed();
}
void KxINetConnection::QueryConnectionStatus(HINTERNET requestHandle, KxINetResponse& response)
{
	DWORD length = INT16_MAX;
	::HttpQueryInfoW(requestHandle, HTTP_QUERY_RAW_HEADERS_CRLF, wxStringBuffer(response.Response, INT16_MAX), &length, 0);
	::HttpQueryInfoW(requestHandle, HTTP_QUERY_STATUS_TEXT, wxStringBuffer(response.Status, INT16_MAX), &length, 0);

	response.Response.Shrink();
	response.Status.Shrink();

	length = sizeof(DWORD);
	::HttpQueryInfoW(requestHandle, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &(response.StatusCode), &length, 0);
	::HttpQueryInfoW(requestHandle, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, &(response.ContentLength), &length, 0);
}

wxString KxINetConnection::GetRequestURL(const KxINetRequest& request) const
{
	return request.ConstructFullURL(m_URLParts.FullPath);
}

KxINetResponse KxINetConnection::DoSendRequest(const wxString& method, KxINetRequest& request, bool test)
{
	DWORD flags = INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_UI|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD;
	if (m_URLParts.ShemeType == INTERNET_SCHEME_HTTP)
	{
		flags |= INTERNET_FLAG_KEEP_CONNECTION;
	}
	else if (m_URLParts.ShemeType == INTERNET_SCHEME_HTTPS)
	{
		flags |= INTERNET_FLAG_SECURE;
	}

	KxINetResponse response;

	wxString fullRequestURL = GetRequestURL(request);
	HINTERNET requestHandle = ::HttpOpenRequestW(m_Handle, KxINet::StringOrNull(method), fullRequestURL.wc_str(), NULL, NULL, KxINet::AcceptTypes, flags, (DWORD_PTR)this);
	if (requestHandle)
	{
		AddRequestHeadres(request, requestHandle);

		// Ask for additional data
		KxINetEvent event(KxEVT_INET_SEND_DATA, this);
		ProcessEvent(event);

		// Send request
		if (::HttpSendRequestW(requestHandle, NULL, NULL, const_cast<void*>(event.GetRequestBuffer()), event.GetRequestBufferSize()))
		{
			// Query main connection info
			QueryConnectionStatus(requestHandle, response);

			// Here user can query additional info
			if (SendQueryEvent(requestHandle))
			{
				// Only test connection
				if (test)
				{
					::InternetCloseHandle(requestHandle);
					return response;
				}

				// Download data
				BYTE tempBuffer[KxINet::TransferBlockSize];

				int64_t totalDownloaded = 0;
				DWORD downloaded = 0;
				do
				{
					clock_t time = std::clock();
					downloaded = 0;
					if (::InternetReadFile(requestHandle, tempBuffer, KxINet::TransferBlockSize, &downloaded))
					{
						response.Data.AppendData(tempBuffer, downloaded);
						totalDownloaded += downloaded;
						time = std::clock() - time;

						KxFileOperationEvent eventDownload(KxEVT_INET_DOWNLOAD);
						eventDownload.SetMinorProcessed(totalDownloaded);
						if (response.ContentLength == 0 || response.ContentLength == (DWORD)-1)
						{
							eventDownload.SetMinorTotal(totalDownloaded);
						}
						else
						{
							eventDownload.SetMinorTotal(response.ContentLength);
						}
						if (time != 0)
						{
							eventDownload.SetSpeed((double)downloaded/(double)time);
						}

						ProcessEvent(eventDownload);
						if (!eventDownload.IsAllowed())
						{
							downloaded = 0;
							break;
						}
					}
				}
				while (downloaded != 0);

				if (totalDownloaded == 0)
				{
					response.Data.Clear();
				}
			}
		}
		else
		{
			QueryConnectionStatus(requestHandle, response);
		}
		::InternetCloseHandle(requestHandle);
	}
	return response;
}

KxINetConnection::KxINetConnection(KxINet* session, const wxString& url, uint16_t port, const wxString& userName, const wxString& password)
	:m_SessionInstance(session), m_URL(url), m_URLParts(KxINet::SplitURL(url)), m_UserName(userName), m_Password(password)
{
	switch (m_URLParts.ShemeType)
	{
		case KxINET_SCHEME_HTTP:
		case KxINET_SCHEME_HTTPS:
		{
			m_ServiceType = KxINET_SERVICE_HTTP;
			break;
		}
		case KxINET_SCHEME_FTP:
		{
			m_ServiceType = KxINET_SERVICE_FTP;
			break;
		}
	};

	if (port == KxINet::DefaultPort)
	{
		switch (m_URLParts.ShemeType)
		{
			case KxINET_SCHEME_HTTP:
			{
				m_Port = KxINet::DefaultPortHTTP;
				break;
			}
			case KxINET_SCHEME_HTTPS:
			{
				m_Port = KxINet::DefaultPortHTTPS;
				break;
			}
		};
	}
	else
	{
		m_Port = port;
	}
}
KxINetConnection::~KxINetConnection()
{
	CloseConnection();
}

bool KxINetConnection::Connect()
{
	CloseConnection();

	DWORD serviceType = INTERNET_SERVICE_HTTP;
	if (m_ServiceType == KxINET_SERVICE_FTP)
	{
		serviceType = INTERNET_SERVICE_FTP;
	}

	m_Handle = ::InternetConnectW(m_SessionInstance->GetHandle(),
								  m_URLParts.HostName.wc_str(),
								  m_Port,
								  KxINet::StringOrNull(m_UserName),
								  KxINet::StringOrNull(m_Password),
								  serviceType,
								  INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD,
								  NULL
	);
	return IsConnected();
}
bool KxINetConnection::CloseConnection()
{
	if (m_Handle != NULL)
	{
		return ::InternetCloseHandle(m_Handle);
	}
	return false;
}

wxString KxINetConnection::GetURL() const
{
	wxString out;
	DWORD length = INTERNET_MAX_URL_LENGTH;
	InternetQueryOptionW(m_Handle, INTERNET_OPTION_URL, wxStringBuffer(out, length), &length);
	return out;
}
void KxINetConnection::SetURL(const wxString& url)
{
	m_URLParts = KxINet::SplitURL(url);
	Disconnect();
}

KxINetConnection& KxINetConnection::operator=(const KxINetConnection& other)
{
	m_SessionInstance = other.m_SessionInstance;
	m_Handle = other.m_Handle;
	m_UserName = other.m_UserName;
	m_Password = other.m_Password;
	m_URLParts = other.m_URLParts;

	m_Port = other.m_Port;
	m_ServiceType = other.m_ServiceType;

	return *this;
}
