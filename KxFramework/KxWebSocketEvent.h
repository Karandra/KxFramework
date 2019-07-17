#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxURI.h"
#include "KxEvent.h"

class KX_API KxWebSocketEvent: public wxNotifyEvent
{
	private:
		KxURI m_URI;
		const void* m_BinaryData = nullptr;
		size_t m_BinarySize = 0;

	public:
		KxWebSocketEvent(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE);
		virtual wxEvent* Clone() const override
		{
			return new KxWebSocketEvent(*this);
		}

	public:
		KxURI GetURI() const
		{
			return m_URI;
		}
		void SetURI(const KxURI& uri)
		{
			m_URI = uri;
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

KxEVENT_DECLARE_GLOBAL(WEBSOCKET_OPEN, KxWebSocketEvent);
KxEVENT_DECLARE_GLOBAL(WEBSOCKET_FAIL, KxWebSocketEvent);
KxEVENT_DECLARE_GLOBAL(WEBSOCKET_CLOSE, KxWebSocketEvent);
KxEVENT_DECLARE_GLOBAL(WEBSOCKET_MESSAGE, KxWebSocketEvent);
KxEVENT_DECLARE_GLOBAL(WEBSOCKET_CONNECTING, KxWebSocketEvent);
