#include "stdafx.h"
#include "KxFramework/KxDualProgressDialog.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxDualProgressDialog, KxStdDialog);

bool KxDualProgressDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
{
	return KxStdDialog::IsEnterAllowed(event, idOut);
}
wxOrientation KxDualProgressDialog::GetViewLabelSizerOrientation() const
{
	return KxStdDialog::GetViewLabelSizerOrientation();
}

KxProgressBar* KxDualProgressDialog::CreateProgressBar()
{
	KxProgressBar* progressBar = new KxProgressBar(m_View, wxID_NONE, 100);
	progressBar->SetInitialSize(FromDIP(wxSize(DefaultProgressWidth, DefaultProgressHeight)));

	return progressBar;
}

bool KxDualProgressDialog::Create(wxWindow* parent,
								  wxWindowID id,
								  const wxString& caption,
								  const wxPoint& pos,
								  const wxSize& size,
								  StdButton buttons,
								  long style
)
{
	m_DialogResizeSide = wxHORIZONTAL;

	if (KxStdDialog::Create(parent, id, caption, pos, size, buttons, style))
	{
		m_View = new KxFramework::UI::Panel(m_ContentPanel, wxID_NONE);
		m_PB1 = CreateProgressBar();
		m_PB2 = CreateProgressBar();

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		m_View->SetSizer(sizer);
		sizer->Add(m_PB1, 0, wxEXPAND);
		sizer->Add(m_PB2, 0, wxEXPAND|wxTOP, FromDIP(wxSize(wxDefaultCoord, 5)).GetHeight());

		PostCreate(pos);
		return true;
	}
	return false;
}
