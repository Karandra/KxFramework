#include "KxStdAfx.h"
#include "KxFramework/KxProgressDialog.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxProgressDialog, KxStdDialog);

bool KxProgressDialog::Create(wxWindow* parent,
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
		wxSize progressBarSize = wxSize(DefaultProgressWidth, DefaultProgressHeight);
		m_View = new KxProgressBar(m_ContentPanel, wxID_NONE, 100);
		m_View->SetInitialSize(progressBarSize);

		PostCreate(pos);
		return true;
	}
	return false;
}
KxProgressDialog::~KxProgressDialog()
{
}

void KxProgressDialog::ResetState()
{
	KxStdDialog::ResetState();
	if (GetProgressBar())
	{
		GetProgressBar()->SetValue(0);
		GetProgressBar()->SetRange(100);
	}
}
bool KxProgressDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
{
	return KxStdDialog::IsEnterAllowed(event, idOut);
}
wxOrientation KxProgressDialog::GetViewLabelSizerOrientation() const
{
	return KxStdDialog::GetViewLabelSizerOrientation();
}
