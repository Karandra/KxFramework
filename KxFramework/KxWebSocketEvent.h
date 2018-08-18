#pragma once
#include "KxFramework/KxFramework.h"

class KxWebSocketEvent: public wxNotifyEvent
{
	private:
		const void* m_BinaryData = NULL;
		size_t m_BinarySize = 0;

	public:
		KxWebSocketEvent(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE);
		virtual wxEvent* Clone() const override
		{
			return new KxWebSocketEvent(*this);
		}

	public:
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

wxDECLARE_EVENT(KxEVT_WEBSOCKET_OPEN, KxWebSocketEvent);
wxDECLARE_EVENT(KxEVT_WEBSOCKET_FAIL, KxWebSocketEvent);
wxDECLARE_EVENT(KxEVT_WEBSOCKET_CLOSE, KxWebSocketEvent);
wxDECLARE_EVENT(KxEVT_WEBSOCKET_MESSAGE, KxWebSocketEvent);
