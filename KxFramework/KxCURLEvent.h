#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileOperationEvent.h"
class KX_API KxCURLSession;
class KX_API KxCURLReplyBase;

class KX_API KxCURLEvent: public KxFileOperationEvent
{
	friend class KxCURLSession;

	private:
		KxCURLSession* m_Session = nullptr;
		KxCURLReplyBase* m_Reply = nullptr;

		const char* m_ResponseData = nullptr;
		size_t m_ResponseLength = 0;

	private:
		void SetResponseData(const char* data, size_t length)
		{
			m_ResponseData = data;
			m_ResponseLength = length;
		}

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

		wxString GetHeaderKey() const;
		wxString GetHeaderValue() const;
		wxString GetHeaderLine() const;

	public:
		wxDECLARE_DYNAMIC_CLASS(KxCURLEvent);
};

KX_DECLARE_EVENT(KxEVT_CURL_DOWNLOAD, KxCURLEvent);
KX_DECLARE_EVENT(KxEVT_CURL_RESPONSE_HEADER, KxCURLEvent);
