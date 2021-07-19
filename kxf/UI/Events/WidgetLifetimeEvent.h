#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class KX_API WidgetLifetimeEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetLifetimeEvent, Created);
			KxEVENT_MEMBER(WidgetLifetimeEvent, Destroyed);

		public:
			WidgetLifetimeEvent() noexcept = default;
			WidgetLifetimeEvent(IWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetLifetimeEvent>(std::move(*this));
			}
	};
}
