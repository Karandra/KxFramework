#pragma once
#include "WidgetItemEvent.h"
#include "../IToolBarWidget.h"
#include "../IToolBarWidgetItem.h"

namespace kxf
{
	class KX_API ToolBarWidgetEvent: public WidgetItemEvent
	{
		public:
			KxEVENT_MEMBER(ToolBarWidgetEvent, DropdownMenu);
			KxEVENT_MEMBER(ToolBarWidgetEvent, OverflowMenu);

			KxEVENT_MEMBER_AS(ToolBarWidgetEvent, Selected, WidgetItemEvent::EvtSelected);
			KxEVENT_MEMBER(ToolBarWidgetEvent, SelectedRightClick);
			KxEVENT_MEMBER(ToolBarWidgetEvent, SelectedMiddleClick);

		public:
			ToolBarWidgetEvent(IToolBarWidget& widget) noexcept
				:WidgetItemEvent(widget)
			{
			}
			ToolBarWidgetEvent(IToolBarWidget& widget, IToolBarWidgetItem& menuItem) noexcept
				:WidgetItemEvent(widget, menuItem)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ToolBarWidgetEvent>(std::move(*this));
			}

			// ToolBarWidgetEvent
			std::shared_ptr<IToolBarWidget> GetToolBarWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IToolBarWidget>();
			}
			std::shared_ptr<IToolBarWidgetItem> GetToolBarWidgetItem() const noexcept
			{
				return m_Item ? m_Item->QueryInterface<IToolBarWidgetItem>() : nullptr;
			}
	};
}
