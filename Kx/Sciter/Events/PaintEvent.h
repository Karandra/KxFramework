#pragma once
#include "Event.h"

namespace KxSciter
{
	class GraphicsContext;
	struct GraphicsContextHandle;
}

namespace KxSciter
{
	class KX_API PaintEvent: public Event
	{
		public:
			KxEVENT_MEMBER(PaintEvent, PaintBackground);
			KxEVENT_MEMBER(PaintEvent, PaintForeground);
			KxEVENT_MEMBER(PaintEvent, PaintOutline);
			KxEVENT_MEMBER(PaintEvent, PaintContent);

		protected:
			GraphicsContextHandle* m_GraphicsContext = nullptr;
			wxRect m_Rect;

		public:
			PaintEvent(Host& host)
				:Event(host)
			{
			}

		public:
			PaintEvent* Clone() const override
			{
				return new PaintEvent(*this);
			}

			GraphicsContext GetGraphicsContext() const;
			void SetGraphicsContext(GraphicsContextHandle* handle);

			wxRect GetRect() const
			{
				return m_Rect;
			}
			void SetRect(const wxRect& rect)
			{
				m_Rect = rect;
			}
	};
}

namespace KxSciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintBackground);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintForeground);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintOutline);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(PaintEvent, PaintContent);
}
