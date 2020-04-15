#include "KxStdAfx.h"
#include "KxFramework/KxStdDialogSimple.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStdDialogSimple, KxStdDialog);

bool KxStdDialogSimple::Create(wxWindow* parent,
							   wxWindowID id,
							   wxWindow* pView,
							   const wxString& caption,
							   const wxPoint& pos,
							   const wxSize& size,
							   StdButton buttons,
							   long style
)
{
	m_View = pView;
	if (KxStdDialog::Create(parent, id, caption, pos, size, buttons, style))
	{
		m_View->Reparent(m_ContentPanel);
		PostCreate(pos);
		return true;
	}
	return false;
}
KxStdDialogSimple::~KxStdDialogSimple()
{
}
