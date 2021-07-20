#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KX_API WidgetKeyEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetKeyEvent, KeyUp);
			KxEVENT_MEMBER(WidgetKeyEvent, KeyDown);

			KxEVENT_MEMBER(WidgetKeyEvent, Char);
			KxEVENT_MEMBER(WidgetKeyEvent, CharHook);

		public:
			WidgetKeyEvent() noexcept = default;
			WidgetKeyEvent(IWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetKeyEvent>(std::move(*this));
			}
	};
}
