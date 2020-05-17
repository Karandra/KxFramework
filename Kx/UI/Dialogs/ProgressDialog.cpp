#include "stdafx.h"
#include "ProgressDialog.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ProgressDialog, StdDialog);

	bool ProgressDialog::Create(wxWindow* parent,
								wxWindowID id,
								const String& caption,
								const wxPoint& pos,
								const wxSize& size,
								StdButton buttons,
								DialogStyle style
	)
	{
		m_DialogResizeSide = wxHORIZONTAL;

		if (StdDialog::Create(parent, id, caption, pos, size, buttons, style))
		{
			m_View = new ProgressBar(m_ContentPanel, wxID_NONE, 100);
			m_View->SetInitialSize(FromDIP(wxSize(DefaultProgressWidth, DefaultProgressHeight)));
			SetProgressBar(m_View);

			PostCreate(pos);
			return true;
		}
		return false;
	}

	bool ProgressDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
	{
		return StdDialog::IsEnterAllowed(event, idOut);
	}
	wxOrientation ProgressDialog::GetViewLabelSizerOrientation() const
	{
		return StdDialog::GetViewLabelSizerOrientation();
	}
}
