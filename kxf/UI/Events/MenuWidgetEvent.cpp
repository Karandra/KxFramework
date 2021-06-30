#include "KxfPCH.h"
#include "MenuWidgetEvent.h"
#include "../IWidget.h"
#include "../IMenuWidget.h"
#include "../IMenuWidgetItem.h"

namespace kxf
{
	MenuWidgetEvent::MenuWidgetEvent(IMenuWidget& menu, std::shared_ptr<IWidget> invokingWidget) noexcept
		:WidgetEvent(menu), m_Menu(std::static_pointer_cast<IMenuWidget>(menu.LockReference())), m_InvokingWidget(std::move(invokingWidget))
	{
	}
	MenuWidgetEvent::MenuWidgetEvent(IMenuWidget& menu, IMenuWidgetItem& menuItem, std::shared_ptr<IWidget> invokingWidget) noexcept
		:MenuWidgetEvent(menu, std::move(invokingWidget))
	{
		m_Item = menuItem.LockReference();
	}
}
