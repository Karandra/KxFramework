#include "stdafx.h"
#include "KxFramework/KxDualInputDialog.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxDualInputDialog, KxStdDialog);

bool KxDualInputDialog::Create(wxWindow* parent,
							   wxWindowID id,
							   const wxString& caption,
							   const wxPoint& pos,
							   const wxSize& size,
							   StdButton buttons,
							   long style
)
{
	if (KxStdDialog::Create(parent, id, caption, pos, size, buttons, style))
	{
		wxSize size(DefaultWidth, wxDefaultCoord);
		const int flags = KxTextBox::DefaultStyle|wxTE_PROCESS_ENTER;

		m_View = new KxFramework::UI::Panel(m_ContentPanel, wxID_NONE);

		m_TextBox1 = new KxTextBox(m_View, wxID_NONE, {}, flags);
		m_TextBox1->SetInitialSize(size);

		m_TextBox2 = new KxTextBox(m_View, wxID_NONE, {}, flags);
		m_TextBox2->SetInitialSize(size);

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		m_View->SetSizer(sizer);
		sizer->Add(m_TextBox1, 0, wxEXPAND);
		sizer->Add(m_TextBox2, 0, wxEXPAND|wxTOP, FromDIP(wxSize(wxDefaultCoord, 5)).GetHeight());

		AddUserWindow(m_TextBox1);
		AddUserWindow(m_TextBox2);
		PostCreate(pos);
		return true;
	}
	return false;
}
