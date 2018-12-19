#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileOperationEvent.h"
class KX_API KxCURLSession;
class KX_API KxCURLReplyBase;

class KX_API KxCURLEvent: public KxFileOperationEvent
{
	private:
		KxCURLSession* m_Session = nullptr;
		KxCURLReplyBase* m_Reply = nullptr;

	public:
		KxCURLEvent() {}
		KxCURLEvent(wxEventType eventType, KxCURLSession* session, KxCURLReplyBase* reply);
		virtual ~KxCURLEvent();
		virtual KxCURLEvent* Clone() const;

	public:
		bool IsOK() const
		{
			return m_Session != nullptr && m_Reply != nullptr;
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

KX_DECLARE_EVENT(KxEVT_CURL_DOWNLOAD, KxCURLEvent);
