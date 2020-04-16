#include "stdafx.h"
#include "KxFramework/KxComboBoxDialog.h"
#include "KxFramework/KxComboBox.h"
#include "KxFramework/KxBitmapComboBox.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxComboBoxDialog, KxStdDialog);

bool KxComboBoxDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
{
	if (GetComboBox()->HasFocus() && event.GetKeyCode() == WXK_RETURN)
	{
		event.Skip(false);
	}
	return KxStdDialog::IsEnterAllowed(event, idOut);
}
wxOrientation KxComboBoxDialog::GetViewLabelSizerOrientation() const
{
	return wxVERTICAL;
}

bool KxComboBoxDialog::Create(wxWindow* parent,
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
		int comboBoxFlags = 0;
		wxSize size(DefaultComboBoxWidth, wxDefaultCoord);
		if (style & KxCBD_BITMAP)
		{
			comboBoxFlags = KxBitmapComboBox::DefaultStyle & ~wxCB_READONLY;
			if (style & KxCBD_READONLY)
			{
				comboBoxFlags |= wxCB_READONLY;
			}

			m_View = new KxBitmapComboBox(m_ContentPanel, wxID_ANY, {}, comboBoxFlags);
		}
		else
		{
			comboBoxFlags = KxComboBox::DefaultStyle & ~wxCB_READONLY;
			if (style & KxCBD_READONLY)
			{
				comboBoxFlags |= wxCB_READONLY;
			}

			m_View = new KxComboBox(m_ContentPanel, wxID_ANY, {}, comboBoxFlags);
		}
		m_View->SetInitialSize(size);
		PostCreate(pos);

		return true;
	}
	return false;
}
