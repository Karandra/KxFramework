#include "KxStdAfx.h"
#include "KxFramework/KxSimplebook.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxSimplebook, wxSimplebook);

bool KxSimplebook::Create(wxWindow* parent,
						  wxWindowID id,
						  long style
)
{
	if (wxSimplebook::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		if (ShouldInheritColours())
		{
			SetBackgroundColour(parent->GetBackgroundColour());
		}
		return true;
	}
	return false;
}
