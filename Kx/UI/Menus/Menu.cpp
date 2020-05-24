#include "stdafx.h"
#include "Menu.h"
#include "MenuEvent.h"
#include "ShellMenu.h"
#include "Kx/Utility/Common.h"

namespace
{
	using namespace KxFramework;
	using namespace KxFramework::UI;

	Menu* g_CurrentMenu = nullptr;

	void wxYieldForCommandsOnly()
	{
		// This right from wxWindowMSW::DoPopupMenu

		// peek all WM_COMMANDs (it will always return WM_QUIT too but we don't
		// want to process it here)
		MSG msg;
		while (::PeekMessageW(&msg, nullptr, WM_COMMAND, WM_COMMAND, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				// if we retrieved a WM_QUIT, insert back into the message queue.
				::PostQuitMessage(0);
				break;
			}

			// luckily (as we don't have access to wxEventLoopImpl method from here
			// anyhow...) we don't need to pre-process WM_COMMANDs so dispatch it
			// immediately
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
	}
	wxWindowID TranslateItemID(wxWindowID id)
	{
		// See KxMenuItem::GetEffectiveID
		if (id < 0)
		{
			return id;
		}
		else
		{
			return id + 1;
		}
	}

	Point TranslateCoordinates(wxWindow* window, const Point& showPos)
	{
		Point pos = showPos;
		if (!pos.IsFullySpecified())
		{
			pos = wxGetMousePosition();
		}
		else if (window)
		{
			pos = window->ClientToScreen(pos);
		}
		return pos;
	}
	Point TranslateCoordinatesAsPopup(wxWindow* window, int offset, uint32_t alignment)
	{
		offset = std::abs(offset);
		Size size = window->GetSize();
		Point pos(0, size.GetHeight() + offset); // TPM_LEFTALIGN|TPM_TOPALIGN == 0
		if (alignment & TPM_RIGHTALIGN)
		{
			pos.X() = size.GetWidth();
		}
		if (alignment & TPM_BOTTOMALIGN)
		{
			pos.Y() = -offset;
		}
		return pos;
	}

	HWND GetParentWindowHandle(wxWindow* window)
	{
		if (window = wxGetTopLevelParent(window))
		{
			return window->GetHandle();
		}
		else if(window = wxTheApp->GetTopWindow())
		{
			return window->GetHandle();
		}
		return nullptr;
	}
	wxWindowID GetSelectedItemID(Menu* menu, uint16_t winID)
	{
		wxWindowID menuWxID = Menu::WinMenuRetToWx(winID);
		MenuItem* item = dynamic_cast<MenuItem*>(menu->wxMenu::FindItem(menuWxID));
		return item ? item->GetId() : wxID_NONE;
	}

	void SendOrQueueEvent(Menu* menu, MenuEvent& event)
	{
		if (wxThread::IsMain())
		{
			menu->ProcessEvent(event);
		}
		else
		{
			menu->QueueEvent(event.Clone());
		}
	}
	void SendOnShowEvent(Menu* menu, const Point& pos)
	{
		MenuEvent event(MenuEvent::EvtOpen, menu);
		event.SetPosition(pos);
		event.SetPopup(true);
		menu->ProcessEvent(event);
	}
	void SendOnCloseEvent(Menu* menu, const Point& pos)
	{
		MenuEvent event(MenuEvent::EvtClose, menu);
		event.SetPosition(pos);
		event.SetPopup(true);
		menu->ProcessEvent(event);
	}
	MenuItem* FindItemByEvent(Menu* menu, const wxEvent& event, Menu** owningMenu = nullptr)
	{
		if (event.GetId() != wxID_NONE)
		{
			return menu->FindItem(event.GetId(), owningMenu);
		}
		return nullptr;
	}

	constexpr uint32_t MapMenuAlignment(FlagSet<Alignment> alignment) noexcept
	{
		uint32_t nativeAlignment = 0;
		Utility::AddFlagRef(nativeAlignment, TPM_LEFTALIGN, alignment & Alignment::Left);
		Utility::AddFlagRef(nativeAlignment, TPM_RIGHTALIGN, alignment & Alignment::Right);
		Utility::AddFlagRef(nativeAlignment, TPM_CENTERALIGN, alignment & Alignment::CenterHorizontal);
		Utility::AddFlagRef(nativeAlignment, TPM_TOPALIGN, alignment & Alignment::Top);
		Utility::AddFlagRef(nativeAlignment, TPM_BOTTOMALIGN, alignment & Alignment::Bottom);
		Utility::AddFlagRef(nativeAlignment, TPM_VCENTERALIGN, alignment & Alignment::CenterVertical);

		return nativeAlignment;
	}

	class ShowMenuScope final
	{
		private:
			Menu& m_Menu;
			wxWindow* m_Window = nullptr;
			HWND m_Handle = nullptr;

		public:
			ShowMenuScope(Menu& menu, wxWindow* window, HWND hWnd)
				:m_Menu(menu), m_Window(window ? window : wxGetActiveWindow()), m_Handle(hWnd)
			{
				if (m_Window)
				{
					m_Menu.SetInvokingWindow(m_Window);
				}
				if (m_Handle)
				{
					SetForegroundWindow(m_Handle);
				}
				m_Menu.UpdateUI();
			}
			~ShowMenuScope()
			{
				if (m_Window)
				{
					m_Menu.SetInvokingWindow(nullptr);
				}
				if (m_Handle)
				{
					::PostMessageW(m_Handle, WM_NULL, 0, 0);
				}
				if (wxThread::IsMain())
				{
					wxYieldForCommandsOnly();
				}
			}
	};
}

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Menu, wxMenu);

	bool Menu::EndMenu()
	{
		return ::EndMenu();
	}
	Menu* Menu::GetCurrentMenu()
	{
		return g_CurrentMenu;
	}

	void Menu::OnSelectItem(wxCommandEvent& event)
	{
		if (event.GetId() != wxID_NONE)
		{
			ProcessItemSelection(event.GetId(), &event);
		}
	}
	void Menu::OnHoverItem(wxMenuEvent& event)
	{
		MenuItem* item = FindItemByEvent(this, event);
		if (item)
		{
			MenuEvent itemEvent(MenuEvent::EvtHover, item->GetMenu(), item);
			itemEvent.SetEventObject(item->GetEventHandler());
			if (!item->ProcessEvent(itemEvent))
			{
				// If item didn't processed this event send it to menu
				item->GetMenu()->ProcessEvent(itemEvent);
			}
		}
		else
		{
			MenuEvent itemEvent(MenuEvent::EvtHover, this);
			itemEvent.SetEventObject(this);
			itemEvent.SetId(event.GetId());
			if (ShellMenu* shellMenu = dynamic_cast<ShellMenu*>(this))
			{
				itemEvent.SetHelpString(shellMenu->GetHelpString(WxIDToWin(event.GetId())));
			}
			ProcessEvent(itemEvent);
		}
		event.Skip(false);
	}

	uint16_t Menu::DoShowMenu(wxWindow* window, const Point& showPos, FlagSet<Alignment> alignment, bool async)
	{
		Point pos = TranslateCoordinates(window, showPos);
		HWND hWnd = GetParentWindowHandle(window);

		ShowMenuScope showMenuScope(*this, window, hWnd);
		SendOnShowEvent(this, pos);

		if (!async)
		{
			g_CurrentMenu = this;
		}

		const int ret = ::TrackPopupMenu(GetHMenu(), (async ? 0 : TPM_RETURNCMD)|TPM_RECURSE|TPM_LEFTBUTTON|MapMenuAlignment(alignment), pos.GetX(), pos.GetY(), 0, hWnd, nullptr);

		if (!async)
		{
			g_CurrentMenu = nullptr;
			SendOnCloseEvent(this, pos);
		}
		return ret;
	}
	void Menu::ProcessItemSelection(int menuWxID, wxCommandEvent* event)
	{
		bool allowSendEvent = true;
		MenuItem* item = dynamic_cast<MenuItem*>(FindItem(menuWxID));
		if (item)
		{
			// Following wxWindow::PopupMenu behavior
			if (item->IsCheckable())
			{
				if (item->GetKind() == wxITEM_RADIO && item->IsChecked())
				{
					allowSendEvent = false;
				}
				else
				{
					item->Check(!item->IsChecked());
				}
			}

			// Try to process event in item first
			if (allowSendEvent)
			{
				MenuEvent menuEvent(MenuEvent::EvtSelect, item->GetMenu(), item);
				menuEvent.SetItem(item);
				menuEvent.SetEventObject(item->GetEventHandler());
				if (event)
				{
					event->Skip(menuEvent.GetSkipped());
				}
				if (item->ProcessEvent(menuEvent) && !menuEvent.GetSkipped())
				{
					return;
				}
			}
		}

		// Process in menu itself if item event handler did not process (or skip) this event
		if (allowSendEvent && item)
		{
			MenuEvent menuEvent(MenuEvent::EvtSelect, item->GetMenu(), item);
			menuEvent.SetEventObject(this);
			menuEvent.SetItem(item);
			item->GetMenu()->ProcessEvent(menuEvent);
			if (event)
			{
				event->Skip(menuEvent.GetSkipped());
			}
		}
	}

	Menu::Menu(const String& label, long style)
		:wxMenu(label, style)
	{
		Bind(wxEVT_MENU, &Menu::OnSelectItem, this);
		Bind(wxEVT_MENU_HIGHLIGHT, &Menu::OnHoverItem, this);
	}

	wxWindowID Menu::Show(wxWindow* window, const Point& pos, FlagSet<Alignment> alignment)
	{
		uint16_t menuWinID = DoShowMenu(window, pos, alignment, false);
		wxWindowID id = GetSelectedItemID(this, menuWinID);

		if (menuWinID != 0)
		{
			ProcessItemSelection(id);
		}
		return id;
	}
	wxWindowID Menu::ShowAsPopup(wxWindow* window, int offset, FlagSet<Alignment> alignment)
	{
		Point pos = TranslateCoordinatesAsPopup(window, offset, alignment);
		return Show(window, pos, alignment);
	}

	MenuItem* Menu::Add(MenuItem* item)
	{
		if (wxMenu::Append(item))
		{
			item->OnAddedToMenu();
			return item;
		}
		return nullptr;
	}
	MenuItem* Menu::Add(Menu* subMenu, const String& label, const String& helpString)
	{
		MenuItem* item = new MenuItem(wxID_ANY, label, helpString, wxITEM_NORMAL);
		item->SetSubMenu(subMenu);
		return Add(item);
	}
	MenuItem* Menu::AddSeparator()
	{
		return Add(new MenuItem(wxID_SEPARATOR, {}, {}, wxITEM_SEPARATOR));
	}

	MenuItem* Menu::Insert(size_t pos, MenuItem* item)
	{
		if (wxMenu::Insert(pos, item))
		{
			item->OnAddedToMenu();
			return item;
		}
		return nullptr;
	}
	MenuItem* Menu::Insert(size_t pos, Menu* subMenu, const String& label, const String& helpString)
	{
		MenuItem* item = new MenuItem(wxID_ANY, label, helpString, wxITEM_NORMAL);
		item->SetSubMenu(subMenu);
		return Insert(pos, item);
	}
	MenuItem* Menu::InsertSeparator(size_t pos)
	{
		return Insert(pos, new MenuItem(wxID_SEPARATOR, {}, {}, wxITEM_SEPARATOR));
	}

	MenuItem* Menu::Prepend(MenuItem* item)
	{
		if (wxMenu::Prepend(item))
		{
			item->OnAddedToMenu();
			return item;
		}
		return nullptr;
	}
	MenuItem* Menu::Prepend(Menu* subMenu, const String& label, const String& helpString)
	{
		MenuItem* item = new MenuItem(wxID_ANY, label, helpString, wxITEM_NORMAL);
		item->SetSubMenu(subMenu);
		return Prepend(item);
	}
	MenuItem* Menu::PrependSeparator()
	{
		return Prepend(new MenuItem(wxID_SEPARATOR, {}, {}, wxITEM_SEPARATOR));
	}

	MenuItem* Menu::FindChildItem(wxWindowID id, size_t* posPtr) const
	{
		return static_cast<MenuItem*>(wxMenu::FindChildItem(TranslateItemID(id), posPtr));
	}
	MenuItem* Menu::FindItem(wxWindowID id, Menu** menu) const
	{
		wxMenu* menuWx = nullptr;
		MenuItem* item = static_cast<MenuItem*>(wxMenu::FindItem(TranslateItemID(id), &menuWx));
		if (menu)
		{
			*menu = static_cast<Menu*>(menuWx);
		}
		return item;
	}
	MenuItem* Menu::FindItemByPosition(size_t pos) const
	{
		return static_cast<MenuItem*>(wxMenu::FindItemByPosition(pos));
	}

	MenuItem* Menu::RemoveItem(MenuItem* item)
	{
		if (item = static_cast<MenuItem*>(wxMenu::Remove(item)))
		{
			item->OnRemovedFromMenu();
			return item;
		}
		return nullptr;
	}
	MenuItem* Menu::RemoveItem(wxWindowID id)
	{
		return RemoveItem(FindItem(id));
	}
}
