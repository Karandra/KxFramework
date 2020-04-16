#include "stdafx.h"
#include "KxFramework/KxPanel.h"
#include "Kx/Drawing/UxTheme.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxPanel, wxPanel);

bool KxPanel::Create(wxWindow* parent,
					 wxWindowID id,
					 long style
)
{
	if (wxPanel::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_DYNAMIC_CLASS(KxDrawablePanel, KxPanel);

const wxBitmap KxDrawablePanel::ms_EmptyBitmap = wxBitmap(8, 8, 32);

wxSize KxDrawablePanel::DrawScaledBitmap(wxGraphicsContext* gc, const wxGraphicsBitmap& bitmap, const wxSize& bitmapSize, const wxRect& rect, ScaleMode scaleMode, double globalScale)
{
	wxSize scaledImageSize;
	double x = 0;
	double y = 0;
	double width = 0;
	double height = 0;

	switch (scaleMode)
	{
		case ScaleMode::Scale_None:
		{
			scaledImageSize = bitmapSize;
			gc->DrawBitmap(!bitmap.IsNull() ? bitmap : gc->CreateBitmap(ms_EmptyBitmap), rect.GetX(), rect.GetY(), bitmapSize.GetWidth(), bitmapSize.GetWidth());
			return scaledImageSize;
		}
		case ScaleMode::Scale_Fill:
		{
			width = rect.GetWidth();
			height = rect.GetHeight();
			break;
		}
		case ScaleMode::Scale_AspectFill:
		case ScaleMode::Scale_AspectFit:
		{
			double scaleFactor;
			double scaleX = (double)rect.GetWidth() / (double)bitmapSize.GetWidth();
			double scaleY = (double)rect.GetHeight() / (double)bitmapSize.GetHeight();
			if ((scaleMode == ScaleMode::Scale_AspectFit && scaleY < scaleX) || (scaleMode == ScaleMode::Scale_AspectFill && scaleY > scaleX))
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

	scaledImageSize.x = width;
	scaledImageSize.y = height;
	gc->DrawBitmap(!bitmap.IsNull() ? bitmap : gc->CreateBitmap(ms_EmptyBitmap), rect.GetX() + x, rect.GetY() + y, width, height);
	return scaledImageSize;
}
wxSize KxDrawablePanel::DrawScaledBitmap(wxGraphicsContext* gc, const wxBitmap& bitmap, const wxRect& rect, ScaleMode scaleMode, double globalScale)
{
	return DrawScaledBitmap(gc, gc->CreateBitmap(bitmap), bitmap.GetSize(), rect, scaleMode, globalScale);
}
wxSize KxDrawablePanel::DrawScaledBitmap(wxWindowDC& dc, const wxBitmap& bitmap, const wxRect& rect, ScaleMode scaleMode, double globalScale)
{
	wxGCDC gcdc(dc);
	return DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, rect, scaleMode, globalScale);
}
wxSize KxDrawablePanel::DrawScaledBitmap(wxMemoryDC& dc, const wxBitmap& bitmap, const wxRect& rect, ScaleMode scaleMode, double globalScale)
{
	wxGCDC gcdc(dc);
	return DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, rect, scaleMode, globalScale);
}
void KxDrawablePanel::DrawTransparencyPattern(wxDC& dc)
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

void KxDrawablePanel::OnDrawBackground(wxEraseEvent& event)
{
	using namespace KxFramework;

	wxDC* dc = event.GetDC();
	dc->SetBackgroundMode(wxBG_STYLE_TRANSPARENT);
	dc->SetBackground(*wxTRANSPARENT_BRUSH);

	auto mode = GetBGMode();
	if (mode & KxPANEL_BG_SOLID)
	{
		dc->SetBackground(GetBackgroundColour());
		dc->Clear();
	}
	else if (mode & KxPANEL_BG_GRADIENT)
	{
		dc->GradientFillLinear(dc->GetSize(), GetForegroundColour(), GetBackgroundColour(), GetGradientDirection());
	}
	else if (mode & KxPANEL_BG_TRANSPARENCY_PATTERN)
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
		UxTheme::DrawParentBackground(*window, *dc, wxRect(wxPoint(0, 0), GetSize()));
	}

	if (mode & KxPANEL_BG_IMAGE)
	{
		m_ScaledImageSize = m_Bitmap.GetSize();
		dc->DrawBitmap(m_Bitmap, wxPoint(0, 0), false);
	}
}
void KxDrawablePanel::OnDrawForeground(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	if (m_BackgroundMode & KxPANEL_FG_IMAGE)
	{
		m_ScaledImageSize = DrawScaledBitmap(dc, m_Bitmap, wxRect(wxPoint(0, 0), GetClientSize()), m_ImageScaleMode, m_ScaleFactor);
	}
	else
	{
		event.Skip();
	}
}

bool KxDrawablePanel::Create(wxWindow* parent,
							 wxWindowID id,
							 long style
)
{
	if (KxPanel::Create(parent, id, style|wxFULL_REPAINT_ON_RESIZE))
	{
		SetBackgroundStyle(wxBG_STYLE_ERASE);
		Bind(wxEVT_ERASE_BACKGROUND, &KxDrawablePanel::OnDrawBackground, this);
		Bind(wxEVT_PAINT, &KxDrawablePanel::OnDrawForeground, this);

		return true;
	}
	return false;
}
