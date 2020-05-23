#include "stdafx.h"
#include "CheckBox.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(CheckBox, wxCheckBox);

	bool CheckBox::Create(wxWindow* parent,
						  wxWindowID id,
						  const String& label,
						  FlagSet<CheckBoxStyle> style
	)
	{
		if (wxCheckBox::Create(parent, id, label, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.ToInt()))
		{
			return true;
		}
		return false;
	}
}
