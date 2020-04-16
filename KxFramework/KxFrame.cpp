#include "stdafx.h"
#include "KxFramework/KxFrame.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxFrame, wxFrame)

bool KxFrame::Create(wxWindow* parent,
					 wxWindowID id,
					 const wxString& title,
					 const wxPoint& pos,
					 const wxSize& size,
					 long style
)
{
	if (wxFrame::Create(parent, id, title, pos, size, style))
	{
		SetStatusBarPane(-1);
		SetDefaultBackgroundColor();
		Center(wxBOTH);

		return true;
	}
	return false;
}
KxFrame::~KxFrame()
{
}
