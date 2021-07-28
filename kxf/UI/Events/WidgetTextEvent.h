#pragma once
#include "WidgetEvent.h"
#include "WidgetMouseEvent.h"

namespace kxf
{
	class URI;
}

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
			WidgetTextEvent(IWidget& widget, const String& text) noexcept
				:WidgetEvent(widget)
			{
				WidgetEvent::SetString(text);
			}
			WidgetTextEvent(IWidget& widget, const URI& uri, WidgetMouseEvent mouseEvent) noexcept;

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetTextEvent>(std::move(*this));
			}

		public:
			// WidgetURIEvent
			URI GetURI() const;
			const WidgetMouseEvent& GetMouseEvent() const noexcept
			{
				return m_MouseEvent;
			}
			WidgetMouseEvent& GetMouseEvent() noexcept
			{
				return m_MouseEvent;
			}

			size_t GetLengthLimit() const noexcept
			{
				return m_LengthLimit;
			}
	};
}
