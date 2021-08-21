#pragma once
#include "WidgetEvent.h"
#include "../IBookWidget.h"

namespace kxf
{
	class KX_API BookWidgetEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(BookWidgetEvent, PageChange);
			KxEVENT_MEMBER(BookWidgetEvent, PageChanged);

			KxEVENT_MEMBER(BookWidgetEvent, PageClose);
			KxEVENT_MEMBER(BookWidgetEvent, PageClosed);

			KxEVENT_MEMBER(BookWidgetEvent, DragAllow);
			KxEVENT_MEMBER(BookWidgetEvent, DragBegin);
			KxEVENT_MEMBER(BookWidgetEvent, DragMove);
			KxEVENT_MEMBER(BookWidgetEvent, DragEnd);
			KxEVENT_MEMBER(BookWidgetEvent, DragDone);

			KxEVENT_MEMBER(BookWidgetEvent, ButtonMiddleUp);
			KxEVENT_MEMBER(BookWidgetEvent, ButtonMiddleDown);

			KxEVENT_MEMBER(BookWidgetEvent, ButtonRightUp);
			KxEVENT_MEMBER(BookWidgetEvent, ButtonRightDown);

			KxEVENT_MEMBER(BookWidgetEvent, PageMenu);
			KxEVENT_MEMBER(BookWidgetEvent, BackgroundDoubleClick);

		private:
			size_t m_OldSelection = IBookWidget::npos;
			size_t m_NewSelection = IBookWidget::npos;

		public:
			BookWidgetEvent(IBookWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}
			BookWidgetEvent(IBookWidget& widget, size_t oldSelection, size_t newSelection) noexcept
				:WidgetEvent(widget), m_OldSelection(oldSelection), m_NewSelection(newSelection)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<BookWidgetEvent>(std::move(*this));
			}

			// ButtonWidgetEvent
			std::shared_ptr<IBookWidget> GetBookWidget() const noexcept
			{
				return GetWidget()->QueryInterface<IBookWidget>();
			}

			size_t GetOldSelection() const noexcept
			{
				return m_OldSelection;
			}
			size_t GetNewSelection() const noexcept
			{
				return m_NewSelection;
			}
	};
}
