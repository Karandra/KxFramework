#pragma once
#include "WidgetEvent.h"
#include "../IButtonWidget.h"

namespace kxf
{
	class KX_API ButtonWidgetEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(ButtonWidgetEvent, Click);
			KxEVENT_MEMBER(ButtonWidgetEvent, Dropdown);

		public:
			ButtonWidgetEvent(IWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ButtonWidgetEvent>(std::move(*this));
			}

			// ButtonWidgetEvent
			std::shared_ptr<IButtonWidget> GetButtonWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IButtonWidget>();
			}
	};
}
