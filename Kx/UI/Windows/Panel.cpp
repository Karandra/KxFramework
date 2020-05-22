#include "stdafx.h"
#include "Panel.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Panel, wxPanel);

	bool Panel::Create(wxWindow* parent,
					   wxWindowID id,
					   WindowStyle style
	)
	{
		if (wxPanel::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), ToInt(style)))
		{
			EnableSystemTheme();
			return true;
		}
		return false;
	}
}
