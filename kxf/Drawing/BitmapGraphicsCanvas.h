#pragma once
#include "Common.h"
#include "GraphicsCanvas.h"
#include "Private/GraphicsCanvas.h"

namespace kxf
{
	class KX_API BitmapGraphicsCanvas: public GraphicsCanvas
	{
		private:
			Drawing::Private::AlphaBitmapRef m_BitmapRef;
			wxMemoryDC m_MemoryDC;
			wxGCDC m_GDC;

		public:
			BitmapGraphicsCanvas(wxBitmap& bitmap, wxGraphicsRenderer* renderer = nullptr)
				:GraphicsCanvas(m_MemoryDC, m_GDC), m_BitmapRef(bitmap), m_MemoryDC(bitmap), m_GDC(&CreateContext(renderer, m_MemoryDC))
			{
				InitCanvas();
			}
			BitmapGraphicsCanvas(wxBitmap& bitmap, wxDC& dc, wxGraphicsRenderer* renderer = nullptr)
				:BitmapGraphicsCanvas(bitmap, renderer)
			{
				m_GDC.CopyAttributes(dc);
				InitCanvas();
			}
			BitmapGraphicsCanvas(wxBitmap& bitmap, wxWindow* window, wxGraphicsRenderer* renderer = nullptr)
				:BitmapGraphicsCanvas(bitmap, renderer)
			{
				wxWindowDC windowDC(window);
				m_GDC.CopyAttributes(windowDC);
				InitCanvas();
			}

		public:
			wxBitmap GetBitmap() override
			{
				Flush();
				return m_BitmapRef.Get();
			}
			Size GetSize() const override
			{
				return m_BitmapRef.Get().GetSize();
			}
	};
}
