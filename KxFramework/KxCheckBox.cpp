#include "stdafx.h"
#include "KxFramework/KxCheckBox.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxCheckBox, wxCheckBox);

bool KxCheckBox::Create(wxWindow* parent,
						wxWindowID id,
						const wxString& label,
						long style
)
{
	if (wxCheckBox::Create(parent, id, label, wxDefaultPosition, wxDefaultSize, style))
	{
		return true;
	}
	return false;
}
