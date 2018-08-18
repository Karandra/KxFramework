#include "KxStdAfx.h"
#include "KxFramework/KxRichToolTip.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxRichToolTip, wxObject);

KxRichToolTip::KxRichToolTip(const wxString& title, const wxString& message)
	:m_ToolTip(title, message), m_Title(title), m_Message(message)
{
	SetIcon(DefaultIcon);
}
KxRichToolTip::~KxRichToolTip()
{
}

void KxRichToolTip::GetBackgroundColour(wxColour* color1, wxColour* color2) const
{
	KxUtility::SetIfNotNull(color1, m_Color1);
	KxUtility::SetIfNotNull(color2, m_Color2);
}
