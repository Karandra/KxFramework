#include "KxStdAfx.h"
#include "KxFramework/KxStyledTextBox.h"
#include "KxFramework/KxStyledTextBox_Scintilla.h"
#include "KxFramework/KxStyledTextBox_SciPosition.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStyledTextBox, wxStyledTextCtrl);

bool KxStyledTextBox::Create(wxWindow* parent,
							 wxWindowID id,
							 long style,
							 const wxValidator& validator
)
{
	if (wxStyledTextCtrl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		SetValidator(validator);

		int style = 0;
		wxFont font = parent->GetFont();
		font.SetFaceName("Consolas");
		StyleSetFont(style, font);
		StyleSetFont(wxSTC_STYLE_LINENUMBER, font);

		SetDoubleBuffered(true);
		SetBufferedDraw(true);
		SetTabWidth(8);
		SetEOLMode(wxSTC_EOL_CRLF);
		SetTechnology(wxSTC_TECHNOLOGY_DIRECTWRITE);
		SetIndentationGuides(4);
		SetMarginType(wxSTC_MARGIN_NUMBER, wxSTC_MARGIN_NUMBER);
		SetMarginWidth(wxSTC_MARGIN_NUMBER, TextWidth(style, "99999"));
		SetWrapMode(wxSTC_WRAP_WORD);

		return true;
	}
	return false;
}
KxStyledTextBox::~KxStyledTextBox()
{
}
