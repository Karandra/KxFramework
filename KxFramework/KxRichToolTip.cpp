#include "stdafx.h"
#include "KxFramework/KxRichToolTip.h"
#include "Kx/Utility/Common.h"

using namespace KxFramework;

wxIMPLEMENT_DYNAMIC_CLASS(KxRichToolTip, wxObject);

KxRichToolTip::KxRichToolTip(const wxString& title, const wxString& message)
	:m_ToolTip(title, message), m_Title(title), m_Message(message)
{
	SetIcon(DefaultIcon);
}

void KxRichToolTip::GetBackgroundColour(wxColour* color1, wxColour* color2) const
{
	Utility::SetIfNotNull(color1, m_Color1);
	Utility::SetIfNotNull(color2, m_Color2);
}
