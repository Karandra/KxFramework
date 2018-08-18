#include "KxStdAfx.h"
#include "KxFramework/KxComboControl.h"
#include "KxFramework/KxComboPopup.h"

wxIMPLEMENT_ABSTRACT_CLASS(KxComboControl, wxComboCtrl)

void KxComboControl::DoShowPopup(const wxRect& rect, int flags)
{
	wxComboCtrl::DoShowPopup(rect, flags);
	m_PopupCtrl->OnDoShowPopup();
}

bool KxComboControl::Create(wxWindow* window,
							wxWindowID id,
							const wxString& value,
							long style,
							const wxValidator& validator
)
{
	if (wxComboCtrl::Create(window, id, value, wxDefaultPosition, wxDefaultSize, style, validator))
	{
		return true;
	}
	return false;
}

bool KxComboControl::ShouldDrawFocus() const
{
	switch (m_FocusDrawMode)
	{
		case DrawFocus::Always:
		{
			return true;
		}
		case DrawFocus::Never:
		{
			return false;
		}
	};
	return wxComboCtrl::ShouldDrawFocus();
}
void KxComboControl::SetFocusDrawMode(DrawFocus mode)
{
	m_FocusDrawMode = mode;
	Refresh();
}
void KxComboControl::SetPopupControl(KxComboPopup* popup)
{
	m_PopupCtrl = popup;
	wxComboCtrl::SetPopupControl(popup);
}
