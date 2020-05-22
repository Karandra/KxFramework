#include "stdafx.h"
#include "StaticBitmap.h"
#include "Kx/Drawing/UxTheme.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(StaticBitmap, wxGenericStaticBitmap);

	void StaticBitmap::OnPaint(wxPaintEvent& event)
	{
		using namespace KxFramework;

		wxBufferedPaintDC dc(this);
		UxTheme::DrawParentBackground(*this, dc, Rect(Point(0, 0), GetSize()));
		event.Skip();
	}

	bool StaticBitmap::Create(wxWindow* parent,
							  wxWindowID id,
							  const wxBitmap& bitmap,
							  WindowStyle style
	)
	{
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		if (wxGenericStaticBitmap::Create(parent, id, bitmap, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), ToInt(style)))
		{
			m_InitialSize = bitmap.IsOk() ? bitmap.GetSize() : this->GetSize();

			SetScaleMode(wxGenericStaticBitmap::Scale_None);
			Bind(wxEVT_PAINT, &StaticBitmap::OnPaint, this);

			return true;
		}
		return false;
	}

	wxImage StaticBitmap::GetImage() const
	{
		return Drawing::ToImage(GetBitmap());
	}
	void StaticBitmap::SetImage(const wxImage& image)
	{
		SetBitmap(Drawing::ToBitmap(image));
	}
}
