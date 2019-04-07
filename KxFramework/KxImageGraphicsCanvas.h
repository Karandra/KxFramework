#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxGraphicsCanvas.h"

class KX_API KxImageGraphicsCanvas: public KxGraphicsCanvas
{
	private:
		class AlphaImageRef
		{
			private:
				wxImage& m_Image;

			private:
				void ClearColor()
				{
					const size_t length = (size_t)m_Image.GetWidth() * (size_t)m_Image.GetHeight() * size_t(3);
					memset(m_Image.GetData(), 0, length);
				}
				void ClearAlpha()
				{
					const size_t length = (size_t)m_Image.GetWidth() * (size_t)m_Image.GetHeight();
					memset(m_Image.GetAlpha(), 0, length);
				}

			public:
				AlphaImageRef(wxImage& image)
					:m_Image(image)
				{
					m_Image.SetMask(false);
					m_Image.SetPalette(wxNullPalette);
					if (!m_Image.HasAlpha())
					{
						m_Image.InitAlpha();
					}

					ClearColor();
					ClearAlpha();
				}

			public:
				const wxImage& Get() const
				{
					return m_Image;
				}
				wxImage& Get()
				{
					return m_Image;
				}
		};

	private:
		AlphaImageRef m_ImageRef;
		wxGCDC m_GDC;

	public:
		KxImageGraphicsCanvas(wxImage& image, wxGraphicsRenderer* renderer = nullptr)
			:KxGraphicsCanvas(m_GDC, m_GDC), m_ImageRef(image), m_GDC(&CreateContext(renderer, image))
		{
			InitCanvas();
		}
		KxImageGraphicsCanvas(wxImage& image, wxDC& dc, wxGraphicsRenderer* renderer = nullptr)
			:KxImageGraphicsCanvas(image, renderer)
		{
			m_GDC.CopyAttributes(dc);
			InitCanvas();
		}
		KxImageGraphicsCanvas(wxImage& image, wxWindow* window, wxGraphicsRenderer* renderer = nullptr)
			:KxImageGraphicsCanvas(image, renderer)
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
		wxSize GetSize() const override
		{
			return m_ImageRef.Get().GetSize();
		}
};
