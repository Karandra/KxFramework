#pragma once
#include "WidgetEvent.h"
#include "../ISearchWidget.h"

namespace kxf
{
	class KX_API SearchWidgetEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(SearchWidgetEvent, Search);
			KxEVENT_MEMBER(SearchWidgetEvent, Cancel);
			KxEVENT_MEMBER(SearchWidgetEvent, Dropdown);

		private:
			String m_SearchQuery;

		public:
			SearchWidgetEvent(ISearchWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}
			SearchWidgetEvent(ISearchWidget& widget, String searchQuery) noexcept
				:WidgetEvent(widget), m_SearchQuery(std::move(searchQuery))
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<SearchWidgetEvent>(std::move(*this));
			}

			// ButtonWidgetEvent
			std::shared_ptr<ISearchWidget> GetSearchWidget() const noexcept
			{
				return GetWidget()->QueryInterface<ISearchWidget>();
			}

			String GetSearchQuery() const
			{
				return m_SearchQuery;
			}
	};
}
