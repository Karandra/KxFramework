#include "KxStdAfx.h"
#include "KxFramework/KxINetEvent.h"
#include "KxFramework/KxFileOperationEvent.h"
#include "KxFramework/KxINet.h"
#include "KxFramework/KxINetConnection.h"
#include <WinINet.h>

KxINetURLParts::KxINetURLParts()
	:Port(KxINet::DefaultPort), ShemeType(KxINET_SCHEME_UNKNOWN)
{
}
//////////////////////////////////////////////////////////////////////////

KxEVENT_DEFINE_GLOBAL(KxINetEvent, INET_SEND_DATA);
KxEVENT_DEFINE_GLOBAL(KxINetEvent, INET_QUERY_INFO);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, INET_DOWNLOAD);

wxIMPLEMENT_DYNAMIC_CLASS(KxINetEvent, KxFileOperationEvent);

KxINetEvent::KxINetEvent(wxEventType type, KxINetConnection* object)
	:wxNotifyEvent(type, 0)
{
	SetEventObject(object);
}
KxINetEvent::~KxINetEvent()
{
}
KxINetEvent* KxINetEvent::Clone() const
{
	return new KxINetEvent(*this);
}

wxString KxINetEvent::QueryInfoString(DWORD infoLevel, DWORD index, DWORD* nextIndex) const
{
	if (IsOK())
	{
		DWORD length = 0;
		DWORD indexOld = index;

		if (!::HttpQueryInfoW(m_RequestHandle, infoLevel, nullptr, &length, &index) && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER && length != 0)
		{
			wxString out;
			index = indexOld;
			if (::HttpQueryInfoW(m_RequestHandle, infoLevel, wxStringBuffer(out, length), &length, &index))
			{
				if (nextIndex)
				{
					if (::GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND || index == indexOld)
					{
						*nextIndex = (DWORD)-1;
					}
					else
					{
						*nextIndex = index;
					}
				}
				return out;
			}
		}
	}
	return wxEmptyString;
}
int64_t KxINetEvent::QueryInfoNumber(DWORD infoLevel, DWORD index, DWORD* nextIndex) const
{
	int64_t value = -1;
	if (IsOK())
	{
		DWORD length = sizeof(value);
		DWORD indexOld = index;
		::HttpQueryInfoW(m_RequestHandle, infoLevel|HTTP_QUERY_FLAG_NUMBER64, &value, &length, &index);

		if (nextIndex)
		{
			if (::GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND || index == indexOld)
			{
				*nextIndex = (DWORD)-1;
			}
			else
			{
				*nextIndex = index;
			}
		}
	}
	return value;
}
wxString KxINetEvent::QueryRequestURL() const
{
	wxString url;
	DWORD urlLength = INTERNET_MAX_URL_LENGTH;
	::InternetQueryOptionW(m_RequestHandle, INTERNET_OPTION_URL, wxStringBuffer(url, urlLength), &urlLength);
	return url;
}
