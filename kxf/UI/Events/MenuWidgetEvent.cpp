#include "KxfPCH.h"
#include "MenuWidgetEvent.h"
#include "../IMenuWidget.h"
#include "../IMenuWidgetItem.h"

namespace kxf
{
	MenuWidgetEvent::MenuWidgetEvent(IMenuWidget& widget, std::shared_ptr<IWidget> invokingWidget) noexcept
		:WidgetEvent(widget), m_InvokingWidget(std::move(invokingWidget))
	{
	}
	MenuWidgetEvent::MenuWidgetEvent(IMenuWidget& widget, IMenuWidgetItem& menuItem, std::shared_ptr<IWidget> invokingWidget) noexcept
		:MenuWidgetEvent(widget, std::move(invokingWidget))
	{
		m_Item = menuItem.LockReference();
	}

	// MenuWidgetEvent
	std::shared_ptr<IMenuWidget> MenuWidgetEvent::GetMenuWidget() const noexcept
	{
		return WidgetEvent::GetWidget()->QueryInterface<IMenuWidget>();
	}
}
