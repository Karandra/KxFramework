#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxURI.h"

class KX_API KxWebSocketEvent: public wxNotifyEvent
{
	private:
		KxURL m_URL;
		const void* m_BinaryData = nullptr;
		size_t m_BinarySize = 0;

	public:
		KxWebSocketEvent(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE);
		virtual wxEvent* Clone() const override
		{
			return new KxWebSocketEvent(*this);
		}

	public:
		KxURL GetURL() const
		{
			return m_URL;
		}
		void SetURL(const KxURL& url)
		{
			m_URL = url;
		}

		wxString GetTextMessage() const
		{
			return GetString();
		}

		const void* GetBinaryData() const
		{
			return m_BinaryData;
		}
		size_t GetBinarySize() const
		{
			return m_BinarySize;
		}
		void SetBinaryMessage(const void* data, size_t size)
		{
			m_BinaryData = data;
			m_BinarySize = size;
		}
};

KX_DECLARE_EVENT(KxEVT_WEBSOCKET_OPEN, KxWebSocketEvent);
KX_DECLARE_EVENT(KxEVT_WEBSOCKET_FAIL, KxWebSocketEvent);
KX_DECLARE_EVENT(KxEVT_WEBSOCKET_CLOSE, KxWebSocketEvent);
KX_DECLARE_EVENT(KxEVT_WEBSOCKET_MESSAGE, KxWebSocketEvent);
KX_DECLARE_EVENT(KxEVT_WEBSOCKET_CONNECTING, KxWebSocketEvent);
