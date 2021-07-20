#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KX_API WidgetFocusEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetFocusEvent, FocusReceived);
			KxEVENT_MEMBER(WidgetFocusEvent, FocusLost);

		public:
			WidgetFocusEvent() noexcept = default;
			WidgetFocusEvent(IWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetFocusEvent>(std::move(*this));
			}
	};
}
