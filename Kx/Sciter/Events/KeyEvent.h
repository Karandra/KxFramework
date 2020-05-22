#pragma once
#include "Event.h"

namespace KxFramework::Sciter
{
	class KX_API KeyEvent: public Event, public wxKeyboardState
	{
		public:
			KxEVENT_MEMBER(KeyEvent, KeyChar);
			KxEVENT_MEMBER(KeyEvent, KeyUp);
			KxEVENT_MEMBER(KeyEvent, KeyDown);

		protected:
			Point m_Position = Point::UnspecifiedPosition();
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
				return m_Position.GetX();
			}
			wxCoord GetY() const
			{
				return m_Position.GetY();
			}
			Point GetPosition() const
			{
				return m_Position;
			}
			void SetPosition(const Point& pos)
			{
				m_Position = pos;
			}
	};
}

namespace KxFramework::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyChar);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyUp);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyDown);
}
