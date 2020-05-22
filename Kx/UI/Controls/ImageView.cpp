#include "stdafx.h"
#include "ImageView.h"
#include "Kx/UI/Windows/DrawablePanel.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ImageView, wxControl);

	void ImageView::DoSetBitmap(const wxGraphicsBitmap& bitmap, const Size& size)
	{
		ScheduleRefresh();

		m_Bitmap = bitmap;
		m_ImageSize = size;
		m_ScaledImageSize = Size(0, 0);
		m_IsAnimation = false;
	}
	
	void ImageView::OnDrawBackground(wxEraseEvent& event)
	{
		wxDC* dc = event.GetDC();
		dc->SetBackgroundMode(wxBG_STYLE_TRANSPARENT);
		dc->SetBackground(*wxTRANSPARENT_BRUSH);

		switch (m_BackgroundMode)
		{
			case ImageViewBackground::Solid:
			{
				dc->SetBackground(GetBackgroundColour());
				dc->Clear();
				break;
			}
			case ImageViewBackground::Gradient:
			{
				dc->GradientFillLinear(GetSize(), GetForegroundColour(), GetBackgroundColour(), m_GradientDirection);
				break;
			}
			case ImageViewBackground::TransparenryPattern:
			{
				DrawablePanel::DrawTransparencyPattern(*dc);
				break;
			}
		};
	}
	void ImageView::OnDrawForeground(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		std::unique_ptr<wxGraphicsContext> context(m_Renderer->CreateContext(dc));

		m_ScaledImageSize = DrawablePanel::DrawScaledBitmap(&*context, m_Bitmap, m_ImageSize, Rect(Point(0, 0), GetClientSize()), m_ScaleMode, m_ScaleFactor);
	}
	void ImageView::OnSize(wxSizeEvent& event)
	{
		Refresh();
		event.Skip();
	}

	bool ImageView::Create(wxWindow* parent,
						   wxWindowID id,
						   WindowStyle style
	)
	{
		if (wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, ToInt(style)))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			if (ShouldInheritColours())
			{
				SetBackgroundColour(parent->GetBackgroundColour());
			}

			SetBackgroundStyle(wxBG_STYLE_ERASE);
			Bind(wxEVT_ERASE_BACKGROUND, &ImageView::OnDrawBackground, this);
			Bind(wxEVT_PAINT, &ImageView::OnDrawForeground, this);
			Bind(wxEVT_SIZE, &ImageView::OnSize, this);

			// Direct2D can't be used right now because it crops images and not scales them.
			m_Renderer = wxGraphicsRenderer::GetGDIPlusRenderer();
			if (m_Renderer == nullptr)
			{
				m_Renderer = wxGraphicsRenderer::GetDefaultRenderer();
			}
			return m_Renderer != nullptr;
		}
		return false;
	}

	void ImageView::SetScaleFactor(double factor)
	{
		if (std::abs(m_ScaleFactor - factor) > 0.01)
		{
			m_ScaleFactor = factor;
			ScheduleRefresh();
		}
	}
	void ImageView::SetBitmap(const wxBitmap& bitmap)
	{
		DoSetBitmap(m_Renderer->CreateBitmap(bitmap), bitmap.GetSize());
	}
	void ImageView::SetBitmap(const wxImage& image)
	{
		DoSetBitmap(m_Renderer->CreateBitmapFromImage(image), image.GetSize());
	}
	void ImageView::SetBitmap(const wxGraphicsBitmap& bitmap, const Size& size)
	{
		DoSetBitmap(bitmap, size);
	}

	void ImageView::LoadFile(const String& filePath, wxBitmapType type, int index)
	{
		wxImage image;
		image.LoadFile(filePath, type, index);
		SetBitmap(image);
	}
	void ImageView::LoadFile(wxInputStream& stream, wxBitmapType type, int index)
	{
		wxImage image;
		image.LoadFile(stream, type, index);
		SetBitmap(image);
	}
}
