#include "KxStdAfx.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxMenuItem.h"
#include "KxFramework/KxMenuEvent.h"
#include "Kx/RTTI/WxRTTI.h"

KxWxRTTI_ImplementClassDynamic2(KxMenuItem, KxMenuItem, wxEvtHandler, wxMenuItem); // wxIMPLEMENT_DYNAMIC_CLASS2(KxMenuItem, wxEvtHandler, wxMenuItem);
wxObject* KxMenuItem::wxCreateObject()
{
	return static_cast<wxEvtHandler*>(new KxMenuItem(wxID_SEPARATOR, {}, {}, wxITEM_SEPARATOR));
}

void KxMenuItem::OnCreate()
{
	CheckIfShouldOwnerDraw();
}
bool KxMenuItem::OnMeasureItem(size_t* width, size_t* height)
{
	const bool result = wxMenuItem::OnMeasureItem(width, height);
	if (const wxWindow* window = GetWindow())
	{
		if (GetKind() != wxITEM_SEPARATOR)
		{
			if (height)
			{
				const wxBitmap& bitmap = GetBitmap(IsChecked());
				const int margin = window->FromDIP(wxSize(wxDefaultCoord, 6)).GetHeight();

				*height = std::max({(int)*height, wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y) + margin, bitmap.GetHeight() + margin});
			}
		}
	}
	return result;
}
bool KxMenuItem::OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status)
{
	return wxMenuItem::OnDrawItem(dc, rect, action, status);
}

void KxMenuItem::OnAddedToMenu()
{
}
void KxMenuItem::OnRemovedFromMenu()
{
}

void KxMenuItem::CheckIfShouldOwnerDraw()
{
	static const bool allowOwnerDraw = wxSystemOptions::GetOptionInt("KxMenu::AllowOwnerDraw") != 0;

	if (!IsOwnerDrawn() && allowOwnerDraw)
	{
		wxWindow* window = GetWindow();
		if (!window)
		{
			window = wxGetActiveWindow();
		}

		if (window && window->GetContentScaleFactor() > 1.0)
		{
			SetOwnerDrawn();
		}
	}
}
wxWindowID KxMenuItem::GetEffectiveID(wxWindowID id) const
{
	return id < 0 ? id : id + 1;
}

KxMenuItem::KxMenuItem(wxWindowID id, const wxString& label, const wxString& helpString, wxItemKind kind)
	:wxMenuItem(nullptr, GetEffectiveID(id), label, helpString, kind, nullptr)
{
	m_EffectiveID = id;
	OnCreate();
}
KxMenuItem::KxMenuItem(const wxString& label, const wxString& helpString, wxItemKind kind)
	:wxMenuItem(nullptr, wxID_ANY, label, helpString, kind, nullptr)
{
	OnCreate();
}
KxMenuItem::~KxMenuItem()
{
}

wxWindow* KxMenuItem::GetWindow() const
{
	if (KxMenu* menu = GetMenu())
	{
		return menu->GetWindow();
	}
	return nullptr;
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
