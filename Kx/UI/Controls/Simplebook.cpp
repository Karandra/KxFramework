#include "stdafx.h"
#include "Simplebook.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Simplebook, wxSimplebook);

	bool Simplebook::Create(wxWindow* parent,
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
}
