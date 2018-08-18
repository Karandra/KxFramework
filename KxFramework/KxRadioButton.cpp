#include "KxStdAfx.h"
#include "KxFramework/KxRadioButton.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxRadioButton, wxRadioButton);

bool KxRadioButton::Create(wxWindow* parent,
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
