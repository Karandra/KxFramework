#pragma once
#include "Event.h"

namespace KxSciter
{
	class KX_API KeyEvent: public Event, public wxKeyboardState
	{
		public:
			KxEVENT_MEMBER(KeyEvent, KeyChar);
			KxEVENT_MEMBER(KeyEvent, KeyUp);
			KxEVENT_MEMBER(KeyEvent, KeyDown);

		protected:
			wxPoint m_Position = wxDefaultPosition;
			wxKeyCode m_KeyCode = wxKeyCode::WXK_NONE;
			wxChar m_UnicodeKey = 0;

		public:
			KeyEvent(Host& host)
				:Event(host)
			{
			}

		public:
			KeyEvent* Clone() const override
			{
				return new KeyEvent(*this);
			}

			wxKeyCode GetKeyCode() const
			{
				return m_KeyCode;
			}
			void SetKeyCode(wxKeyCode keyCode)
			{
				m_KeyCode = keyCode;
			}

			wxChar GetUnicodeKey() const
			{
				return m_UnicodeKey;
			}
			void SetUnicodeKey(wxChar unicodeKey)
			{
				m_UnicodeKey = unicodeKey;
			}
	
			wxCoord GetX() const
			{
				return m_Position.x;
			}
			wxCoord GetY() const
			{
				return m_Position.y;
			}
			wxPoint GetPosition() const
			{
				return m_Position;
			}
			void SetPosition(const wxPoint& pos)
			{
				m_Position = pos;
			}
	};
}

namespace KxSciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyChar);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyUp);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyDown);
}
