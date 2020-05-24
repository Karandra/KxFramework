#pragma once
#include "../Common.h"
#include "../URI.h"
#include "Kx/EventSystem/Event.h"

namespace kxf
{
	class KX_API WebSocketEvent: public wxNotifyEvent
	{
		public:
			KxEVENT_MEMBER(WebSocketEvent, Open);
			KxEVENT_MEMBER(WebSocketEvent, Fail);
			KxEVENT_MEMBER(WebSocketEvent, Close);
			KxEVENT_MEMBER(WebSocketEvent, Message);
			KxEVENT_MEMBER(WebSocketEvent, Connecting);

		private:
			URI m_URI;
			const void* m_BinaryData = nullptr;
			size_t m_BinarySize = 0;

		public:
			WebSocketEvent(EventID type = Event::EvtNull, int winid = wxID_NONE)
				:wxNotifyEvent(type, winid)
			{
				Allow();
			}

		public:
			WebSocketEvent* Clone() const override
			{
				return new WebSocketEvent(*this);
			}

			URI GetURI() const
			{
				return m_URI;
			}
			void SetURI(const URI& uri)
			{
				m_URI = uri;
			}

			wxString GetString() const = delete;
			void SetString(const wxString&) = delete;

			String GetTextMessage() const
			{
				return wxNotifyEvent::GetString();
			}
			void SetTextMessage(const String& message)
			{
				wxNotifyEvent::SetString(message);
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

		public:
			wxDECLARE_DYNAMIC_CLASS(WebSocketEvent);
	};
}
