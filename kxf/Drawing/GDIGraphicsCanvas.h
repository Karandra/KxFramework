#pragma once
#include "GDICanvas.h"
#include "GDIWindowCanvas.h"
#include "GDIMemoryCanvas.h"
#include <wx/dcgraph.h>

namespace kxf
{
	class KX_API GDIGraphicsCanvas final: public GDICanvas
	{
		private:
			wxGCDC m_DC;

		public:
			GDIGraphicsCanvas()
				:GDICanvas(m_DC)
			{
			}
			GDIGraphicsCanvas(GDIMemoryCanvas& dc)
				:GDICanvas(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsCanvas(GDIWindowCanvas& dc)
				:GDICanvas(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsCanvas(GDIWindowPaintCanvas& dc)
				:GDICanvas(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsCanvas(GDIBufferedPaintCanvas& dc)
				:GDICanvas(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsCanvas(GDIAutoBufferedPaintCanvas& dc)
				:GDICanvas(m_DC), m_DC(dc.ToWxDC())
			{
			}
			GDIGraphicsCanvas(std::unique_ptr<wxGraphicsContext> graphicsContext)
				:GDICanvas(m_DC), m_DC(graphicsContext.release())
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
