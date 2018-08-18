#include "KxStdAfx.h"
#include "KxFramework/KxCURLEvent.h"
#include "KxFramework/KxCURL.h"

wxDEFINE_EVENT(KxEVT_CURL_DOWNLOAD, KxCURLEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxCURLEvent, KxFileOperationEvent);

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
