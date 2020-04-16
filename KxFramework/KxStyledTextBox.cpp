#include "stdafx.h"
#include "KxFramework/KxStyledTextBox.h"
#include "KxFramework/KxStyledTextBox_Scintilla.h"
#include "KxFramework/KxStyledTextBox_SciPosition.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStyledTextBox, wxStyledTextCtrl);

namespace
{
	constexpr int g_GlobalStyle = 0;
	constexpr int g_DefaultStyle = wxSTC_STYLE_DEFAULT;
}

bool KxStyledTextBox::Create(wxWindow* parent,
							 wxWindowID id,
							 long style,
							 const wxValidator& validator
)
{
	if (wxStyledTextCtrl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		SetValidator(validator);

		wxFont font = parent->GetFont();
		font.SetFaceName("Consolas");
		StyleSetFont(style, font);
		StyleSetFont(wxSTC_STYLE_LINENUMBER, font);

		SetDoubleBuffered(true);
		SetBufferedDraw(true);
		SetTabWidth(8);
		SetEOLMode(wxSTC_EOL_CRLF);
		SetTechnology(wxSTC_TECHNOLOGY_DEFAULT);
		SetIndentationGuides(4);
		SetMarginType(wxSTC_MARGIN_NUMBER, wxSTC_MARGIN_NUMBER);
		SetMarginWidth(wxSTC_MARGIN_NUMBER, TextWidth(g_GlobalStyle, "99999"));
		SetWrapMode(wxSTC_WRAP_WORD);

		m_IsCreated = true;
		return true;
	}
	return false;
}
KxStyledTextBox::~KxStyledTextBox()
{
}

bool KxStyledTextBox::SetBackgroundColour(const wxColour& color)
{
	if (m_IsCreated)
	{
		StyleSetBackground(g_GlobalStyle, color);
		StyleSetBackground(g_DefaultStyle, color);
		SetWhitespaceBackground(true, color);
	}
	return wxStyledTextCtrl::SetBackgroundColour(color);
}
bool KxStyledTextBox::SetForegroundColour(const wxColour& color)
{
	if (m_IsCreated)
	{
		StyleSetForeground(g_GlobalStyle, color);
		StyleSetForeground(g_DefaultStyle, color);
		SetWhitespaceForeground(true, color);
	}
	return wxStyledTextCtrl::SetForegroundColour(color);
}
