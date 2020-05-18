#include "stdafx.h"
#include "Menu.h"
#include "MenuItem.h"
#include "MenuEvent.h"
#include "Kx/wxWidgets/RTTI.h"
#include <wx/sysopt.h>

namespace KxFramework::UI
{
	KxWxRTTI_ImplementClassDynamic2(MenuItem, MenuItem, wxEvtHandler, wxMenuItem); // wxIMPLEMENT_DYNAMIC_CLASS2(KxMenuItem, wxEvtHandler, wxMenuItem);
	wxObject* MenuItem::wxCreateObject()
	{
		return static_cast<wxEvtHandler*>(new MenuItem(wxID_SEPARATOR, {}, {}, wxITEM_SEPARATOR));
	}

	void MenuItem::OnCreate()
	{
		CheckIfShouldOwnerDraw();
	}
	bool MenuItem::OnMeasureItem(size_t* width, size_t* height)
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
	bool MenuItem::OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status)
	{
		return wxMenuItem::OnDrawItem(dc, rect, action, status);
	}

	void MenuItem::OnAddedToMenu()
	{
	}
	void MenuItem::OnRemovedFromMenu()
	{
	}

	void MenuItem::CheckIfShouldOwnerDraw()
	{
		static const bool allowOwnerDraw = wxSystemOptions::GetOptionInt("Kx::UI::Menu::AllowOwnerDraw") != 0;

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
	wxWindowID MenuItem::GetEffectiveID(wxWindowID id) const
	{
		return id < 0 ? id : id + 1;
	}

	MenuItem::MenuItem(wxWindowID id, const String& label, const String& helpString, wxItemKind kind)
		:wxMenuItem(nullptr, GetEffectiveID(id), label, helpString, kind, nullptr)
	{
		m_EffectiveID = id;
		OnCreate();
	}
	MenuItem::MenuItem(const String& label, const String& helpString, wxItemKind kind)
		: wxMenuItem(nullptr, wxID_ANY, label, helpString, kind, nullptr)
	{
		OnCreate();
	}

	wxWindow* MenuItem::GetWindow() const
	{
		if (Menu* menu = GetMenu())
		{
			return menu->GetWindow();
		}
		return nullptr;
	}
	Menu* MenuItem::GetMenu() const
	{
		return static_cast<Menu*>(GetMenuWx());
	}
	Menu* MenuItem::GetSubMenu() const
	{
		return static_cast<Menu*>(GetSubMenuWx());
	}

	void MenuItem::SetMenu(Menu* menu)
	{
		wxMenuItem::SetMenu(menu);
	}
	void MenuItem::SetSubMenu(Menu* subMenu)
	{
		wxMenuItem::SetSubMenu(subMenu);
	}

	void MenuItem::SetBitmap(const wxBitmap& normal, const wxBitmap& checked)
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
	bool MenuItem::SetDefault()
	{
		return ::SetMenuDefaultItem(GetMenu()->GetHMenu(), Menu::WxIDToWin(GetId()), FALSE);
	}
}
