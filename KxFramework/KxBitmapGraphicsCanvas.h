#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxGraphicsCanvas.h"

class KX_API KxBitmapGraphicsCanvas: public KxGraphicsCanvas
{
	private:
		class AlphaBitmapRef
		{
			private:
				wxBitmap& m_Bitmap;

			public:
				AlphaBitmapRef(wxBitmap& bitmap)
					:m_Bitmap(bitmap)
				{
					m_Bitmap.UseAlpha(true);
				}

			public:
				const wxBitmap& Get() const
				{
					return m_Bitmap;
				}
				wxBitmap& Get()
				{
					return m_Bitmap;
				}
		};

	private:
		AlphaBitmapRef m_BitmapRef;
		wxMemoryDC m_MemoryDC;
		wxGCDC m_GDC;

	public:
		KxBitmapGraphicsCanvas(wxBitmap& bitmap, wxGraphicsRenderer* renderer = nullptr)
			:KxGraphicsCanvas(m_MemoryDC, m_GDC), m_BitmapRef(bitmap), m_MemoryDC(bitmap), m_GDC(&CreateContext(renderer, m_MemoryDC))
		{
			InitCanvas();
		}
		KxBitmapGraphicsCanvas(wxBitmap& bitmap, wxDC& dc, wxGraphicsRenderer* renderer = nullptr)
			:KxBitmapGraphicsCanvas(bitmap, renderer)
		{
			m_GDC.CopyAttributes(dc);
			InitCanvas();
		}
		KxBitmapGraphicsCanvas(wxBitmap& bitmap, wxWindow* window, wxGraphicsRenderer* renderer = nullptr)
			:KxBitmapGraphicsCanvas(bitmap, renderer)
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
		wxSize GetSize() const override
		{
			return m_BitmapRef.Get().GetSize();
		}
};
