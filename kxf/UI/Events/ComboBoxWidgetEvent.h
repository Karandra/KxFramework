#pragma once
#include "WidgetEvent.h"
#include "../IComboBoxWidget.h"

namespace kxf
{
	class KX_API ComboBoxWidgetEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(ComboBoxWidgetEvent, ItemSelected);
			KxEVENT_MEMBER(ComboBoxWidgetEvent, DropdownShow);
			KxEVENT_MEMBER(ComboBoxWidgetEvent, DropdownDismiss);

		private:
			size_t m_Index = std::numeric_limits<size_t>::max();

		public:
			ComboBoxWidgetEvent(IComboBoxWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}
			ComboBoxWidgetEvent(IComboBoxWidget& widget, size_t index) noexcept
				:WidgetEvent(widget), m_Index(index)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ComboBoxWidgetEvent>(std::move(*this));
			}

			// ButtonWidgetEvent
			std::shared_ptr<IComboBoxWidget> GetButtonWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IComboBoxWidget>();
			}
	};
}
