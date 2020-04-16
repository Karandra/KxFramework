#include "stdafx.h"
#include "KxFramework/KxProgressDialog.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxProgressDialog, KxStdDialog);

bool KxProgressDialog::Create(wxWindow* parent,
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
		m_View = new KxProgressBar(m_ContentPanel, wxID_NONE, 100);
		m_View->SetInitialSize(FromDIP(wxSize(DefaultProgressWidth, DefaultProgressHeight)));
		SetProgressBar(m_View);

		PostCreate(pos);
		return true;
	}
	return false;
}

bool KxProgressDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
{
	return KxStdDialog::IsEnterAllowed(event, idOut);
}
wxOrientation KxProgressDialog::GetViewLabelSizerOrientation() const
{
	return KxStdDialog::GetViewLabelSizerOrientation();
}
