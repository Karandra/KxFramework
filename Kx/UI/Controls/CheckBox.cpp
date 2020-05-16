#include "stdafx.h"
#include "CheckBox.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(CheckBox, wxCheckBox);

	bool CheckBox::Create(wxWindow* parent,
						  wxWindowID id,
						  const String& label,
						  CheckBoxStyle style
	)
	{
		if (wxCheckBox::Create(parent, id, label, wxDefaultPosition, wxDefaultSize, ToInt(style)))
		{
			return true;
		}
		return false;
	}
}
