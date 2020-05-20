#include "stdafx.h"
#include "TextBoxDialog.h"
#include "Kx/Utility/Common.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(TextBoxDialog, StdDialog);

	bool TextBoxDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
	{
		if (IsMultiLine() || GetStyledTextBox() != nullptr)
		{
			Utility::SetIfNotNull(idOut, wxID_NONE);
			return false;
		}
		return StdDialog::IsEnterAllowed(event, idOut);
	}
	wxOrientation TextBoxDialog::GetViewLabelSizerOrientation() const
	{
		return StdDialog::GetViewLabelSizerOrientation();
	}
	wxOrientation TextBoxDialog::GetWindowResizeSide() const
	{
		if (m_DialogResizeSide == -1)
		{
			if (IsMultiLine())
			{
				return wxBOTH;
			}
			else
			{
				return wxHORIZONTAL;
			}
		}
		else
		{
			return m_DialogResizeSide;
		}
	}

	bool TextBoxDialog::Create(wxWindow* parent,
							   wxWindowID id,
							   const String& caption,
							   const wxPoint& pos,
							   const wxSize& size,
							   StdButton buttons,
							   TextBoxDialogStyle style
	)
	{
		m_DialogResizeSide = static_cast<wxOrientation>(-1);
		m_Options = style;

		if (StdDialog::Create(parent, id, caption, pos, size, buttons, EnumClass::Combine<DialogStyle>(style)))
		{
			wxSize size(DefaultWidth, wxDefaultCoord);

			if (style & TextBoxDialogStyle::Styled)
			{
				StyledTextBox* textBox = new StyledTextBox(m_ContentPanel, wxID_NONE);
				textBox->SetMultiline(style & TextBoxDialogStyle::Multiline);
				textBox->SetEditable(!(style & TextBoxDialogStyle::ReadOnly));
				if (!textBox->IsMultiline())
				{
					textBox->SetInsertionPointEnd();
					textBox->SelectAll();
				}
				m_View = textBox;
			}
			else
			{
				TextBoxStyle flags = TextBoxStyle::ProcessTab;
				if (style & TextBoxDialogStyle::Multiline)
				{
					flags |= TextBoxStyle::Multiline;
					size.SetWidth(DefaultMLWidth);
					size.SetHeight(DefaultMLHeight);
				}
				else
				{
					flags |= TextBoxStyle::ProcessEnter;
				}
				if (style & TextBoxDialogStyle::Password)
				{
					flags |= TextBoxStyle::Password;
				}

				TextBox* textBox = new TextBox(m_ContentPanel, wxID_NONE, {}, flags);
				textBox->SetEditable(!(style & TextBoxDialogStyle::ReadOnly));
				if (!textBox->IsMultiLine())
				{
					textBox->SetInsertionPointEnd();
					textBox->SelectAll();
				}
				m_View = textBox;
			}
			m_View->SetMinSize(size);
			m_View->SetFocus();

			PostCreate(pos);
			return true;
		}
		return false;
	}
}
