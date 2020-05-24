#include "stdafx.h"
#include "ComboBoxDialog.h"
#include "kxf/UI/Controls/ComboBox.h"
#include "kxf/UI/Controls/BitmapComboBox.h"

namespace kxf::UI
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
								const Point& pos,
								const Size& size,
								FlagSet<StdButton> buttons,
								FlagSet<ComboBoxDialogStyle> style
	)
	{
		m_DialogResizeSide = wxHORIZONTAL;

		if (StdDialog::Create(parent, id, caption, pos, size, buttons, CombineFlags<DialogStyle>(*style)))
		{
			const Size size(DefaultComboBoxWidth, wxDefaultCoord);
			const FlagSet<ComboBoxStyle> comboBoxFlags = ComboBox::DefaultStyle.Clone().Mod(ComboBoxStyle::ReadOnly, style & ComboBoxDialogStyle::ReadOnly);

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
