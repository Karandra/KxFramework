#pragma once
#include "WidgetEvent.h"
#include "../IWidgetItem.h"

namespace kxf
{
	class KX_API WidgetItemEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetItemEvent, Click);
			KxEVENT_MEMBER(WidgetItemEvent, Enter);
			KxEVENT_MEMBER(WidgetItemEvent, Leave);

		protected:
			std::shared_ptr<IWidgetItem> m_Item;
			WidgetID m_ItemID;

		public:
			WidgetItemEvent(IWidget& owningWidget, WidgetID id = {}) noexcept
				:WidgetEvent(owningWidget), m_ItemID(id)
			{
			}
			WidgetItemEvent(IWidget& owningWidget, IWidgetItem& widgetItem) noexcept
				:WidgetEvent(owningWidget), m_Item(widgetItem.QueryInterface<IWidgetItem>())
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetItemEvent>(std::move(*this));
			}

			// WidgetItemEvent
			std::shared_ptr<IWidgetItem> GetWidgetItem() const noexcept
			{
				return m_Item;
			}
			WidgetID GetID() const noexcept
			{
				return m_ItemID;
			}
	};
}
