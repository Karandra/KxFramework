#include "stdafx.h"
#include "KxFramework/KxStaticBitmap.h"
#include "Kx/Drawing/UxTheme.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStaticBitmap, wxGenericStaticBitmap);

void KxStaticBitmap::OnPaint(wxPaintEvent& event)
{
	using namespace KxFramework;

	wxBufferedPaintDC dc(this);
	UxTheme::DrawParentBackground(*this, dc, wxRect(wxPoint(0, 0), GetSize()));
	event.Skip();
}

bool KxStaticBitmap::Create(wxWindow* parent,
							wxWindowID id,
							const wxBitmap& bitmap,
							long style
)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	if (wxGenericStaticBitmap::Create(parent, id, bitmap, wxDefaultPosition, wxDefaultSize, style))
	{
		m_InitialSize = bitmap.IsOk() ? bitmap.GetSize() : this->GetSize();

		SetScaleMode(wxGenericStaticBitmap::Scale_None);
		Bind(wxEVT_PAINT, &KxStaticBitmap::OnPaint, this);

		return true;
	}
	return false;
}
