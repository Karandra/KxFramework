#include "KxStdAfx.h"
#include "KxFramework/KxParagraph.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxParagraph, KxHTMLWindow);

bool KxParagraph::DoSetValue(const wxString& text)
{
	return KxHTMLWindow::SetValue(wxString::Format("<span>%s</span>", ProcessPlainText(text)));
}

bool KxParagraph::Create(wxWindow* parent,
						 wxWindowID id,
						 const wxString& text,
						 long style
)
{
	if (KxHTMLWindow::Create(parent, id, text, style))
	{
		DoSetValue(text);
		return true;
	}
	return false;
}
KxParagraph::~KxParagraph()
{
}

const wxString& KxParagraph::GetValue() const
{
	return m_Text;
}
bool  KxParagraph::SetValue(const wxString& text)
{
	return DoSetValue(text);
}
