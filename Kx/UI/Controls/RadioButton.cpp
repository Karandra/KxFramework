#include "stdafx.h"
#include "RadioButton.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(RadioButton, wxRadioButton);

	bool RadioButton::Create(wxWindow* parent,
							 wxWindowID id,
							 const wxString& label,
							 long style
	)
	{
		if (wxRadioButton::Create(parent, id, label, wxDefaultPosition, wxDefaultSize, style))
		{
			return true;
		}
		return false;
	}
}
