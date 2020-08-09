#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KX_API KeyEvent: public SciterEvent, public wxKeyboardState
	{
		public:
			KxEVENT_MEMBER(KeyEvent, KeyChar);
			KxEVENT_MEMBER(KeyEvent, KeyUp);
			KxEVENT_MEMBER(KeyEvent, KeyDown);

		protected:
			Point m_Position = Point::UnspecifiedPosition();
			wxKeyCode m_KeyCode = wxKeyCode::WXK_NONE;
			wxUniChar m_UnicodeKey;

		public:
			KeyEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<KeyEvent>(std::move(*this));
			}

			// SciterEvent
			wxKeyCode GetKeyCode() const
			{
				return m_KeyCode;
			}
			void SetKeyCode(wxKeyCode keyCode)
			{
				m_KeyCode = keyCode;
			}

			wxUniChar GetUnicodeKey() const
			{
				return m_UnicodeKey;
			}
			void SetUnicodeKey(wxUniChar unicodeKey)
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

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyChar);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyUp);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(KeyEvent, KeyDown);
}
