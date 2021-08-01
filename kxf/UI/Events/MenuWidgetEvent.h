#pragma once
#include "WidgetEvent.h"
#include "../IMenuWidget.h"
#include "../IMenuWidgetItem.h"

namespace kxf
{
	class KX_API MenuWidgetEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(MenuWidgetEvent, Open);
			KxEVENT_MEMBER(MenuWidgetEvent, Close);
			KxEVENT_MEMBER(MenuWidgetEvent, Select);
			KxEVENT_MEMBER(MenuWidgetEvent, Hover);

		private:
			std::shared_ptr<IMenuWidgetItem> m_Item;
			std::shared_ptr<IWidget> m_InvokingWidget;
			Point m_PopupPosition = Point::UnspecifiedPosition();

		public:
			MenuWidgetEvent(IMenuWidget& widget, std::shared_ptr<IWidget> invokingWidget = nullptr) noexcept
				:WidgetEvent(widget), m_InvokingWidget(std::move(invokingWidget))
			{
			}
			MenuWidgetEvent(IMenuWidget& widget, IMenuWidgetItem& menuItem, std::shared_ptr<IWidget> invokingWidget = nullptr) noexcept
				:WidgetEvent(widget), m_InvokingWidget(std::move(invokingWidget)), m_Item(menuItem.QueryInterface<IMenuWidgetItem>())
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<MenuWidgetEvent>(std::move(*this));
			}

			// MenuWidgetEvent
			std::shared_ptr<IMenuWidget> GetMenuWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IMenuWidget>();
			}
			std::shared_ptr<IMenuWidgetItem> GetMenuWidgetItem() const noexcept
			{
				return m_Item;
			}
			std::shared_ptr<IWidget> GetInvokingWidget() const noexcept
			{
				return m_InvokingWidget;
			}
			
			Point GetPopupPosition() const noexcept
			{
				return m_PopupPosition;
			}
			void SetPopupPosition(const Point& position) noexcept
			{
				m_PopupPosition = position;
			}
	};
}
