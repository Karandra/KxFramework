#include "KxStdAfx.h"
#include "KxFramework/KxNotebook.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxNotebook, wxNotebook);

bool KxNotebook::Create(wxWindow* parent,
						wxWindowID id,
						long style
)
{
	if (wxNotebook::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		return true;
	}
	return false;
}
