#pragma once
#include "WidgetEvent.h"
#include "WidgetMouseEvent.h"
#include "kxf/Network/URI.h"

namespace kxf
{
	class KX_API WidgetTextEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetTextEvent, LengthLimit);
			KxEVENT_MEMBER(WidgetTextEvent, Changed);
			KxEVENT_MEMBER(WidgetTextEvent, Commit);
			KxEVENT_MEMBER(WidgetTextEvent, URI);

			KxEVENT_MEMBER(WidgetTextEvent, Cut);
			KxEVENT_MEMBER(WidgetTextEvent, Copy);
			KxEVENT_MEMBER(WidgetTextEvent, Paste);

		private:
			WidgetMouseEvent m_MouseEvent;
			URI m_URI;
			String m_Text;
			size_t m_LengthLimit = String::npos;

		public:
			WidgetTextEvent() noexcept = default;
			WidgetTextEvent(IWidget& widget) noexcept
				:WidgetEvent(widget)
			{
			}
			WidgetTextEvent(IWidget& widget, size_t lengthLimit) noexcept
				:WidgetEvent(widget), m_LengthLimit(lengthLimit)
			{
			}
			WidgetTextEvent(IWidget& widget, String text) noexcept
				:WidgetEvent(widget), m_Text(std::move(text))
			{
			}
			WidgetTextEvent(IWidget& widget, URI uri, WidgetMouseEvent mouseEvent) noexcept
				:WidgetEvent(widget), m_MouseEvent(std::move(mouseEvent)), m_URI(std::move(uri))
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetTextEvent>(std::move(*this));
			}

		public:
			// WidgetURIEvent
			const WidgetMouseEvent& GetMouseEvent() const noexcept
			{
				return m_MouseEvent;
			}
			WidgetMouseEvent& GetMouseEvent() noexcept
			{
				return m_MouseEvent;
			}

			URI GetURI() const
			{
				return m_URI;
			}
			String GetText() const
			{
				return m_Text;
			}
			size_t GetLengthLimit() const noexcept
			{
				return m_LengthLimit;
			}
	};
}
