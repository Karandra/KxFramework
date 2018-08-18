#include "KxStdAfx.h"
#include "KxFramework/KxListBoxDialog.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxListBoxDialog, KxStdDialog);

void KxListBoxDialog::ResetState()
{
	KxStdDialog::ResetState();
	if (GetListBox())
	{
		GetListBox()->ClearItems();
	}
}
bool KxListBoxDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
{
	return KxStdDialog::IsEnterAllowed(event, idOut);
}
wxOrientation KxListBoxDialog::GetViewLabelSizerOrientation() const
{
	return KxStdDialog::GetViewLabelSizerOrientation();
}

bool KxListBoxDialog::Create(wxWindow* parent,
							 wxWindowID id,
							 const wxString& caption,
							 const wxPoint& pos,
							 const wxSize& size,
							 int buttons,
							 long style
)
{
	if (KxStdDialog::Create(parent, id, caption, pos, size, buttons, style))
	{
		m_View = new KxListBox(m_ContentPanel, wxID_ANY);
		PostCreate(pos);
		return true;
	}
	return false;
}
KxListBoxDialog::~KxListBoxDialog()
{
}
