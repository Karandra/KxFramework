#pragma once
#include "WidgetEvent.h"

namespace kxf
{
	class IGraphicsContext;
}

namespace kxf
{
	class KX_API WidgetDrawEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetDrawEvent, DrawContent);
			KxEVENT_MEMBER(WidgetDrawEvent, DrawBackground);

		protected:
			std::shared_ptr<IGraphicsContext> m_GraphicsContext;

		public:
			WidgetDrawEvent() noexcept = default;
			WidgetDrawEvent(IWidget& widget, std::shared_ptr<IGraphicsContext> gc) noexcept
				:WidgetEvent(widget), m_GraphicsContext(std::move(gc))
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetDrawEvent>(std::move(*this));
			}

		public:
			// WidgetDrawEvent
			virtual std::shared_ptr<IGraphicsContext> GetGraphicsContext()
			{
				return m_GraphicsContext;
			}
	};
}
