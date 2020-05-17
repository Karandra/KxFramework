#include "stdafx.h"
#include "ComboBoxDialog.h"
#include "Kx/UI/Controls/ComboBox.h"
#include "Kx/UI/Controls/BitmapComboBox.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ComboBoxDialog, StdDialog);

	bool ComboBoxDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
	{
		if (GetComboBox()->HasFocus() && event.GetKeyCode() == WXK_RETURN)
		{
			event.Skip(false);
		}
		return StdDialog::IsEnterAllowed(event, idOut);
	}
	wxOrientation ComboBoxDialog::GetViewLabelSizerOrientation() const
	{
		return wxVERTICAL;
	}

	bool ComboBoxDialog::Create(wxWindow* parent,
								wxWindowID id,
								const String& caption,
								const wxPoint& pos,
								const wxSize& size,
								StdButton buttons,
								ComboBoxDialogStyle style
	)
	{
		m_DialogResizeSide = wxHORIZONTAL;

		if (StdDialog::Create(parent, id, caption, pos, size, buttons, EnumClass::Combine<DialogStyle>(style)))
		{
			const wxSize size(DefaultComboBoxWidth, wxDefaultCoord);
			const ComboBoxStyle comboBoxFlags = Utility::ModFlag(ComboBox::DefaultStyle, ComboBoxStyle::ReadOnly, style & ComboBoxDialogStyle::ReadOnly);

			if (style & ComboBoxDialogStyle::Bitmap)
			{
				m_View = new BitmapComboBox(m_ContentPanel, wxID_ANY, {}, comboBoxFlags);
			}
			else
			{
				m_View = new ComboBox(m_ContentPanel, wxID_ANY, {}, comboBoxFlags);
			}
			m_View->SetInitialSize(size);
			PostCreate(pos);

			return true;
		}
		return false;
	}
}
