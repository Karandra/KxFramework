#include "KxStdAfx.h"
#include "KxFramework/KxTextBoxDialog.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxTextBoxDialog, KxStdDialog);

bool KxTextBoxDialog::IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut) const
{
	if (IsMultiLine() || GetStyledTextBox() != NULL)
	{
		KxUtility::SetIfNotNull(idOut, wxID_NONE);
		return false;
	}
	return KxStdDialog::IsEnterAllowed(event, idOut);
}
wxOrientation KxTextBoxDialog::GetViewLabelSizerOrientation() const
{
	return KxStdDialog::GetViewLabelSizerOrientation();
}
wxOrientation KxTextBoxDialog::GetWindowResizeSide() const
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

bool KxTextBoxDialog::Create(wxWindow* parent,
							 wxWindowID id,
							 const wxString& caption,
							 const wxPoint& pos,
							 const wxSize& size,
							 int buttons,
							 long style
)
{
	m_DialogResizeSide = (wxOrientation)-1;
	m_Options = (KxTBD_Options)(style & KxTBD_MASK);

	if (KxStdDialog::Create(parent, id, caption, pos, size, buttons, KxUtility::ModFlag(style, KxTBD_MASK, false)))
	{
		wxSize size(DefaultWidth, wxDefaultCoord);

		if (style & KxTBD_STYLED)
		{
			KxStyledTextBox* textBox = new KxStyledTextBox(m_ContentPanel, wxID_NONE);
			textBox->SetMultiLine(style & KxTBD_MULTILINE);
			textBox->SetEditable(!(style & KxTBD_READONLY));
			if (!textBox->IsMultiLine())
			{
				textBox->SetInsertionPointEnd();
				textBox->SelectAll();
			}
			m_View = textBox;
		}
		else
		{
			int flags = KxTextBox::DefaultStyle|wxTE_PROCESS_TAB;
			if (style & KxTBD_MULTILINE)
			{
				flags |= wxTE_MULTILINE;
				size.SetWidth(DefaultMLWidth);
				size.SetHeight(DefaultMLHeight);
			}
			else
			{
				flags |= wxTE_PROCESS_ENTER;
			}
			if (style & KxTBD_PASSWORD)
			{
				flags |= wxTE_PASSWORD;
			}

			KxTextBox* textBox = new KxTextBox(m_ContentPanel, wxID_NONE, wxEmptyString, flags);
			textBox->SetEditable(!(style & KxTBD_READONLY));
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
KxTextBoxDialog::~KxTextBoxDialog()
{
}
