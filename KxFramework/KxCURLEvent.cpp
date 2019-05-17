#include "KxStdAfx.h"
#include "KxFramework/KxCURLEvent.h"
#include "KxFramework/KxCURL.h"

wxDEFINE_EVENT(KxEVT_CURL_DOWNLOAD, KxCURLEvent);
wxDEFINE_EVENT(KxEVT_CURL_RESPONSE_HEADER, KxCURLEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxCURLEvent, KxFileOperationEvent);

//////////////////////////////////////////////////////////////////////////
namespace
{
	void NormalizeValue(wxString& value)
	{
		KxString::Trim(value, true, true);
		value.Replace(wxS("\r"), wxEmptyString, true);
		value.Replace(wxS("\n"), wxEmptyString, true);
	}

	using CharTraits = std::char_traits<char>;
}

//////////////////////////////////////////////////////////////////////////
KxCURLEvent::KxCURLEvent(wxEventType eventType, KxCURLSession* session, KxCURLReplyBase* reply)
	:KxFileOperationEvent(eventType), m_Session(session), m_Reply(reply)
{
}
KxCURLEvent::~KxCURLEvent()
{
}
KxCURLEvent* KxCURLEvent::Clone() const
{
	return new KxCURLEvent(*this);
}

wxString KxCURLEvent::GetHeaderName() const
{
	if (m_ResponseData)
	{
		if (const char* colon = CharTraits::find(m_ResponseData, m_ResponseLength, ':'))
		{
			wxString value = wxString::FromUTF8(m_ResponseData, colon - m_ResponseData);
			NormalizeValue(value);
			return value;
		}
	}
	return {};
}
wxString KxCURLEvent::GetHeaderValue() const
{
	if (m_ResponseData)
	{
		if (const char* colon = CharTraits::find(m_ResponseData, m_ResponseLength, ':'))
		{
			// Skip colon itself and one space after it
			constexpr size_t offset = 2;

			wxString value = wxString::FromUTF8(colon + offset, m_ResponseLength - (colon - m_ResponseData) - offset);
			NormalizeValue(value);
			return value;
		}
	}
	return {};
}
wxString KxCURLEvent::GetHeaderLine() const
{
	if (m_ResponseData)
	{
		wxString value = wxString::FromUTF8(m_ResponseData, m_ResponseLength);
		NormalizeValue(value);
		return value;
	}
	return {};
}
