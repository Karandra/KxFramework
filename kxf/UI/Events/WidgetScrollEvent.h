#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KX_API WidgetScrollEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetScrollEvent, Top);
			KxEVENT_MEMBER(WidgetScrollEvent, Bottom);
			KxEVENT_MEMBER(WidgetScrollEvent, LineUp);
			KxEVENT_MEMBER(WidgetScrollEvent, LineDown);
			KxEVENT_MEMBER(WidgetScrollEvent, PageUp);
			KxEVENT_MEMBER(WidgetScrollEvent, PageDown);

			KxEVENT_MEMBER(WidgetScrollEvent, ThumbTrack);
			KxEVENT_MEMBER(WidgetScrollEvent, ThumbRelease);

		private:
			Orientation m_Orientation = Orientation::None;
			int m_Value = -1;

		public:
			WidgetScrollEvent() noexcept = default;
			WidgetScrollEvent(IWidget& widget, Orientation orientation, int value) noexcept
				:WidgetEvent(widget), m_Orientation(orientation), m_Value(value)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetScrollEvent>(std::move(*this));
			}

		public:
			// WidgetScrollEvent
			Orientation GetOrientation() const noexcept
			{
				return m_Orientation;
			}
			int GetValue() const noexcept
			{
				return m_Value;
			}
	};
}
