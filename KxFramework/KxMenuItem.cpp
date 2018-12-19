#include "KxStdAfx.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxMenuItem.h"
#include "KxFramework/KxMenuEvent.h"
#include "KxFramework/KxWxRTTI.h"

KxWxRTTI_ImplementClassDynamic2(KxMenuItem, KxMenuItem, wxEvtHandler, wxMenuItem); // wxIMPLEMENT_DYNAMIC_CLASS2(KxMenuItem, wxEvtHandler, wxMenuItem);
wxObject* KxMenuItem::wxCreateObject()
{
	return static_cast<wxEvtHandler*>(new KxMenuItem(wxID_SEPARATOR, wxEmptyString, wxEmptyString, wxITEM_SEPARATOR, nullptr));
}

wxWindowID KxMenuItem::GetEffectiveID(wxWindowID id) const
{
	return id < 0 ? id : id + 1;
}

KxMenuItem::KxMenuItem(wxWindowID id = wxID_SEPARATOR, const wxString& label, const wxString& helpString, wxItemKind kind, KxMenu* subMenu)
	:wxMenuItem(nullptr, GetEffectiveID(id), label, helpString, kind, subMenu)
{
	m_EffectiveID = id;
}
KxMenuItem::KxMenuItem(const wxString& label, const wxString& helpString, wxItemKind kind)
	:wxMenuItem(nullptr, wxID_ANY, label, helpString, kind, nullptr)
{
}
KxMenuItem::~KxMenuItem()
{
}

KxMenu* KxMenuItem::GetMenu() const
{
	return static_cast<KxMenu*>(GetWxMenu());
}
KxMenu* KxMenuItem::GetSubMenu() const
{
	return static_cast<KxMenu*>(GetWxSubMenu());
}

void KxMenuItem::SetMenu(KxMenu* menu)
{
	wxMenuItem::SetMenu(menu);
}
void KxMenuItem::SetSubMenu(KxMenu* subMenu)
{
	wxMenuItem::SetSubMenu(subMenu);
}

void KxMenuItem::SetBitmap(const wxBitmap& normal, const wxBitmap& checked)
{
	if (IsCheckable())
	{
		if (checked.IsOk())
		{
			wxMenuItem::SetBitmaps(normal, checked);
		}
		else
		{
			wxMenuItem::SetBitmap(normal, false);
		}
	}
	else
	{
		wxMenuItem::SetBitmaps(normal, checked);
	}
}
bool KxMenuItem::SetDefault()
{
	return ::SetMenuDefaultItem(GetMenu()->GetHMenu(), KxMenu::WxIDToWin(GetId()), FALSE);
}
