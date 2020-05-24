#pragma once
#include "Common.h"
#include "GraphicsCanvas.h"
#include "Private/GraphicsCanvas.h"

namespace kxf
{
	class KX_API ImageGraphicsCanvas: public GraphicsCanvas
	{
		private:
			Drawing::Private::AlphaImageRef m_ImageRef;
			wxGCDC m_GDC;

		public:
			ImageGraphicsCanvas(wxImage& image, wxGraphicsRenderer* renderer = nullptr)
				:GraphicsCanvas(m_GDC, m_GDC), m_ImageRef(image), m_GDC(&CreateContext(renderer, image))
			{
				InitCanvas();
			}
			ImageGraphicsCanvas(wxImage& image, wxDC& dc, wxGraphicsRenderer* renderer = nullptr)
				:ImageGraphicsCanvas(image, renderer)
			{
				m_GDC.CopyAttributes(dc);
				InitCanvas();
			}
			ImageGraphicsCanvas(wxImage& image, wxWindow* window, wxGraphicsRenderer* renderer = nullptr)
				:ImageGraphicsCanvas(image, renderer)
			{
				wxWindowDC windowDC(window);
				m_GDC.CopyAttributes(windowDC);
				InitCanvas();
			}

		public:
			wxImage GetImage()
			{
				Flush();
				return m_ImageRef.Get();
			}
			wxBitmap GetBitmap() override
			{
				Flush();
				return wxBitmap(m_ImageRef.Get(), 32);
			}
			Size GetSize() const override
			{
				return m_ImageRef.Get().GetSize();
			}
	};
}
