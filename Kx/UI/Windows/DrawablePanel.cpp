#include "stdafx.h"
#include "DrawablePanel.h"
#include "Kx/Drawing/UxTheme.h"
#include <wx/bookctrl.h>

namespace
{
	const wxBitmap g_EmptyBitmap = wxBitmap(8, 8, 32);
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(DrawablePanel, Panel);

	Size DrawablePanel::DrawScaledBitmap(wxGraphicsContext* gc, const wxGraphicsBitmap& bitmap, const Size& bitmapSize, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		Size scaledImageSize;
		double x = 0;
		double y = 0;
		double width = 0;
		double height = 0;

		switch (scaleMode)
		{
			case BitmapScaleMode::None:
			{
				scaledImageSize = bitmapSize;
				gc->DrawBitmap(!bitmap.IsNull() ? bitmap : gc->CreateBitmap(g_EmptyBitmap), rect.GetX(), rect.GetY(), bitmapSize.GetWidth(), bitmapSize.GetWidth());
				return scaledImageSize;
			}
			case BitmapScaleMode::Fill:
			{
				width = rect.GetWidth();
				height = rect.GetHeight();
				break;
			}
			case BitmapScaleMode::AspectFit:
			case BitmapScaleMode::AspectFill:
			{
				double scaleFactor;
				double scaleX = (double)rect.GetWidth() / (double)bitmapSize.GetWidth();
				double scaleY = (double)rect.GetHeight() / (double)bitmapSize.GetHeight();
				if ((scaleMode == BitmapScaleMode::AspectFit && scaleY < scaleX) || (scaleMode == BitmapScaleMode::AspectFill && scaleY > scaleX))
				{
					scaleFactor = scaleY;
				}
				else
				{
					scaleFactor = scaleX;
				}
				scaleFactor *= globalScale;

				width = bitmapSize.GetWidth() * scaleFactor;
				height = bitmapSize.GetHeight() * scaleFactor;
				break;
			}
		};

		x = (rect.GetWidth() - width) / 2;
		y = (rect.GetHeight() - height) / 2;

		scaledImageSize.X() = width;
		scaledImageSize.Y() = height;
		gc->DrawBitmap(!bitmap.IsNull() ? bitmap : gc->CreateBitmap(g_EmptyBitmap), rect.GetX() + x, rect.GetY() + y, width, height);
		return scaledImageSize;
	}
	Size DrawablePanel::DrawScaledBitmap(wxGraphicsContext* gc, const wxBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		return DrawScaledBitmap(gc, gc->CreateBitmap(bitmap), bitmap.GetSize(), rect, scaleMode, globalScale);
	}
	Size DrawablePanel::DrawScaledBitmap(wxWindowDC& dc, const wxBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		wxGCDC gcdc(dc);
		return DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, rect, scaleMode, globalScale);
	}
	Size DrawablePanel::DrawScaledBitmap(wxMemoryDC& dc, const wxBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale)
	{
		wxGCDC gcdc(dc);
		return DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, rect, scaleMode, globalScale);
	}
	void DrawablePanel::DrawTransparencyPattern(wxDC& dc)
	{
		const wxWindow* window = dc.GetWindow();

		const int sideSize = window->FromDIP(8);
		dc.SetBackground(window->GetBackgroundColour());
		dc.SetBrush(window->GetForegroundColour());
		dc.SetPen(window->GetForegroundColour());
		dc.Clear();

		size_t indexY = 0;
		for (size_t y = 0; y <= (size_t)dc.GetSize().GetHeight(); y += sideSize)
		{
			for (size_t x = indexY % 2 == 0 ? 0 : sideSize; x <= (size_t)dc.GetSize().GetWidth(); x += 2 * sideSize)
			{
				dc.DrawRectangle(x, y, sideSize, sideSize);
			}
			indexY++;
		}
	}

	void DrawablePanel::OnDrawBackground(wxEraseEvent& event)
	{
		wxDC* dc = event.GetDC();
		dc->SetBackgroundMode(wxBG_STYLE_TRANSPARENT);
		dc->SetBackground(*wxTRANSPARENT_BRUSH);

		auto mode = GetBGMode();
		if (mode & DrawablePanelMode::Soild)
		{
			dc->SetBackground(GetBackgroundColour());
			dc->Clear();
		}
		else if (mode & DrawablePanelMode::Gradient)
		{
			dc->GradientFillLinear(dc->GetSize(), GetForegroundColour(), GetBackgroundColour(), GetGradientDirection());
		}
		else if (mode & DrawablePanelMode::TransparencyPattern)
		{
			DrawTransparencyPattern(*dc);
		}
		else
		{
			wxWindow* window = this;
			if (GetParent()->IsKindOf(wxCLASSINFO(wxBookCtrlBase)))
			{
				window = GetParent();
			}
			UxTheme::DrawParentBackground(*window, *dc, Rect(Point(0, 0), GetSize()));
		}

		if (mode & DrawablePanelMode::BGImage)
		{
			m_ScaledImageSize = m_Bitmap.GetSize();
			dc->DrawBitmap(m_Bitmap, Point(0, 0), false);
		}
	}
	void DrawablePanel::OnDrawForeground(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		if (m_BackgroundMode & DrawablePanelMode::FGImage)
		{
			m_ScaledImageSize = DrawScaledBitmap(dc, m_Bitmap, Rect(Point(0, 0), GetClientSize()), m_ImageScaleMode, m_ScaleFactor);
		}
		else
		{
			event.Skip();
		}
	}

	bool DrawablePanel::Create(wxWindow* parent,
							   wxWindowID id,
							   FlagSet<WindowStyle> style
	)
	{
		if (Panel::Create(parent, id, style|WindowStyle::FullRepaintOnResize))
		{
			SetBackgroundStyle(wxBG_STYLE_ERASE);
			Bind(wxEVT_ERASE_BACKGROUND, &DrawablePanel::OnDrawBackground, this);
			Bind(wxEVT_PAINT, &DrawablePanel::OnDrawForeground, this);

			return true;
		}
		return false;
	}
}
