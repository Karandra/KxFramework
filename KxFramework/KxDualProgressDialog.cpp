#include "KxStdAfx.h"
#include "KxFramework/KxDualProgressDialog.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxDualProgressDialog, KxStdDialog);

void KxDualProgressDialog::ResetState()
{
	KxStdDialog::ResetState();
	if (GetPB1())
	{
		GetPB1()->SetValue(0);
		GetPB1()->SetRange(100);
	}

	if (GetPB2())
	{
		GetPB2()->SetValue(0);
		GetPB2()->SetRange(100);
	}
}
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
	auto progressBar = new KxProgressBar(m_View, wxID_NONE, 100);
	progressBar->SetInitialSize(wxSize(DefaultProgressWidth, DefaultProgressHeight));

	return progressBar;
}

bool KxDualProgressDialog::Create(wxWindow* parent,
								  wxWindowID id,
								  const wxString& caption,
								  const wxPoint& pos,
								  const wxSize& size,
								  int buttons,
								  long style
)
{
	m_DialogResizeSide = wxHORIZONTAL;

	if (KxStdDialog::Create(parent, id, caption, pos, size, buttons, style))
	{
		m_View = new KxPanel(m_ContentPanel, wxID_NONE);
		m_PB1 = CreateProgressBar();
		m_PB2 = CreateProgressBar();

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		m_View->SetSizer(sizer);
		sizer->Add(m_PB1, 0, wxEXPAND);
		sizer->Add(m_PB2, 0, wxEXPAND|wxTOP, 5);

		PostCreate(pos);
		return true;
	}
	return false;
}
KxDualProgressDialog::~KxDualProgressDialog()
{
}
