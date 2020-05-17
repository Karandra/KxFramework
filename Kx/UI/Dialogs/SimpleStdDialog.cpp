#include "stdafx.h"
#include "SimpleStdDialog.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(SimpleStdDialog, StdDialog);

	bool SimpleStdDialog::Create(wxWindow* parent,
								   wxWindowID id,
								   wxWindow* pView,
								   const wxString& caption,
								   const wxPoint& pos,
								   const wxSize& size,
								   StdButton buttons,
								   DialogStyle style
	)
	{
		m_View = pView;
		if (StdDialog::Create(parent, id, caption, pos, size, buttons, style))
		{
			m_View->Reparent(m_ContentPanel);
			PostCreate(pos);
			return true;
		}
		return false;
	}
}
