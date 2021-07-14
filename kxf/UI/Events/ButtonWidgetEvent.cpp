#include "KxfPCH.h"
#include "ButtonWidgetEvent.h"
#include "../IButtonWidget.h"

namespace kxf
{
	ButtonWidgetEvent::ButtonWidgetEvent(IButtonWidget& widget) noexcept
		:WidgetEvent(widget)
	{
	}

	// ButtonWidgetEvent
	std::shared_ptr<IButtonWidget> ButtonWidgetEvent::GetButtonWidget() const noexcept
	{
		return WidgetEvent::GetWidget()->QueryInterface<IButtonWidget>();
	}
}
