#pragma once
#include "Event.h"

namespace kxf::Sciter
{
	class KX_API MouseEvent: public Event, public wxMouseState
	{
		public:
			KxEVENT_MEMBER(MouseEvent, MouseEnter);
			KxEVENT_MEMBER(MouseEvent, MouseLeave);
			KxEVENT_MEMBER(MouseEvent, MouseMove);
			KxEVENT_MEMBER(MouseEvent, MouseUp);
			KxEVENT_MEMBER(MouseEvent, MouseDown);

			KxEVENT_MEMBER(MouseEvent, MouseClick);
			KxEVENT_MEMBER(MouseEvent, MouseDoubleClick);
			KxEVENT_MEMBER(MouseEvent, MouseTick);
			KxEVENT_MEMBER(MouseEvent, MouseIdle);

		protected:
			Point m_RelativePosition = Point::UnspecifiedPosition();
			wxStockCursor m_CursorType = wxStockCursor::wxCURSOR_NONE;
			bool m_IsOnIcon = false;

		public:
			MouseEvent(Host& host)
				:Event(host)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<MouseEvent>(std::move(*this));
			}

			Point GetRelativePosition() const
			{
				return m_RelativePosition;
			}
			void SetRelativePosition(const Point& pos)
			{
				m_RelativePosition = pos;
			}
	
			wxStockCursor GetCursorType() const
			{
				return m_CursorType;
			}
			void SetCursorType(wxStockCursor cursor)
			{
				m_CursorType = cursor;
			}

			bool IsOnIcon() const
			{
				return m_IsOnIcon;
			}
			void SetIsOnIcon(bool onIcon = true)
			{
				m_IsOnIcon = onIcon;
			}
	};
}

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseEnter);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseLeave);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseMove);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseUp);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseDown);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseClick);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseDoubleClick);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseTick);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(MouseEvent, MouseIdle);
}
