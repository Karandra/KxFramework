#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileOperationEvent.h"
class KxCURLSession;
class KxCURLReplyBase;

class KxCURLEvent: public KxFileOperationEvent
{
	private:
		KxCURLSession* m_Session = NULL;
		KxCURLReplyBase* m_Reply = NULL;

	public:
		KxCURLEvent() {}
		KxCURLEvent(wxEventType eventType, KxCURLSession* session, KxCURLReplyBase* reply);
		virtual ~KxCURLEvent();
		virtual KxCURLEvent* Clone() const;

	public:
		bool IsOK() const
		{
			return m_Session != NULL && m_Reply != NULL;
		}

		KxCURLSession& GetSession() const
		{
			return *m_Session;
		}
		KxCURLReplyBase& GetReply() const
		{
			return *m_Reply;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxCURLEvent);
};

wxDECLARE_EVENT(KxEVT_CURL_DOWNLOAD, KxCURLEvent);
