#include "KxStdAfx.h"
#include "KxFramework/KxDualInputDialog.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxDualInputDialog, KxStdDialog);

bool KxDualInputDialog::Create(wxWindow* parent,
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
		wxSize size(DefaultWidth, wxDefaultCoord);
		int flags = KxTextBox::DefaultStyle|wxTE_PROCESS_ENTER;

		m_View = new KxPanel(m_ContentPanel, wxID_NONE);

		m_Input1 = new KxTextBox(m_View, wxID_NONE, wxEmptyString, flags);
		m_Input1->SetInitialSize(size);

		m_Input2 = new KxTextBox(m_View, wxID_NONE, wxEmptyString, flags);
		m_Input2->SetInitialSize(size);

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		m_View->SetSizer(sizer);
		sizer->Add(m_Input1, 0, wxEXPAND);
		sizer->Add(m_Input2, 0, wxEXPAND|wxTOP, 5);

		AddUserWindow(m_Input1);
		AddUserWindow(m_Input2);
		PostCreate(pos);
		return true;
	}
	return false;
}
KxDualInputDialog::~KxDualInputDialog()
{
}

void KxDualInputDialog::SetInputFocus(int index)
{
	switch (index)
	{
		case 0:
		{
			m_Input1->SetFocus();
			break;
		}
		case 1:
		{
			m_Input2->SetFocus();
			break;
		}
	};
}
