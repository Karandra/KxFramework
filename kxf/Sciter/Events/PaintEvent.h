#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class GraphicsContext;
	struct GraphicsContextHandle;
}

namespace kxf::Sciter
{
	class KX_API PaintEvent: public SciterEvent
	{
		public:
			KxEVENT_MEMBER(PaintEvent, PaintBackground);
			KxEVENT_MEMBER(PaintEvent, PaintForeground);
			KxEVENT_MEMBER(PaintEvent, PaintOutline);
			KxEVENT_MEMBER(PaintEvent, PaintContent);

		protected:
			GraphicsContextHandle* m_GraphicsContext = nullptr;
			Rect m_Rect;

		public:
			PaintEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<PaintEvent>(std::move(*this));
			}

			// SciterEvent
			GraphicsContext GetGraphicsContext() const;
			void SetGraphicsContext(GraphicsContextHandle* handle);

			Rect GetRect() const
			{
				return m_Rect;
			}
			void SetRect(const Rect& rect)
			{
				m_Rect = rect;
			}
	};
}

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintBackground);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintForeground);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintOutline);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintContent);
}
