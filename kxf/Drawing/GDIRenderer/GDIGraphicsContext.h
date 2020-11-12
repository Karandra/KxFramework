#pragma once
#include "GDIContext.h"
#include "GDIWindowContext.h"
#include "GDIMemoryContext.h"
#include <wx/dcgraph.h>

namespace kxf
{
	class KX_API GDIGraphicsContext final: public GDIContext
	{
		private:
			wxGCDC m_DC;

		public:
			GDIGraphicsContext()
				:GDIContext(m_DC)
			{
			}
			GDIGraphicsContext(GDIMemoryContext& dc)
				:GDIContext(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsContext(GDIWindowContext& dc)
				:GDIContext(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsContext(GDIPaintContext& dc)
				:GDIContext(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsContext(GDIBufferedPaintContext& dc)
				:GDIContext(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsContext(GDIAutoBufferedPaintContext& dc)
				:GDIContext(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsContext(std::unique_ptr<wxGraphicsContext> graphicsContext)
				:GDIContext(m_DC), m_DC(graphicsContext.release())
			{
			}

		public:
			const wxGCDC& ToWxDC() const noexcept
			{
				return m_DC;
			}
			wxGCDC& ToWxDC() noexcept
			{
				return m_DC;
			}
	};
}
