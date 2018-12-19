#include "KxStdAfx.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxMenuEvent.h"
#include "KxFramework/KxShellMenu.h"
#include <thread>

namespace
{
	KxMenu* ms_CurrentMenu = nullptr;

	void wxYieldForCommandsOnly()
	{
		// This right from wxWindowMSW::DoPopupMenu

		// peek all WM_COMMANDs (it will always return WM_QUIT too but we don't
		// want to process it here)
		MSG msg;
		while (::PeekMessageW(&msg, (HWND)nullptr, WM_COMMAND, WM_COMMAND, PM_REMOVE))
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

	wxPoint TranslateCoordinates(wxWindow* window, const wxPoint& showPos)
	{
		wxPoint pos = showPos;
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
	wxPoint TranslateCoordinatesAsPopup(wxWindow* window, int offset, DWORD alignment)
	{
		offset = std::abs(offset);
		wxSize size = window->GetSize();
		wxPoint pos(0, size.GetHeight() + offset); // TPM_LEFTALIGN|TPM_TOPALIGN == 0
		if (alignment & TPM_RIGHTALIGN)
		{
			pos.x = size.GetWidth();
		}
		if (alignment & TPM_BOTTOMALIGN)
		{
			pos.y = -offset;
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
	wxWindowID GetSelectedItemID(KxMenu* menu, WORD winID)
	{
		wxWindowID menuWxID = KxMenu::WinMenuRetToWx(winID);
		KxMenuItem* item = dynamic_cast<KxMenuItem*>(menu->wxMenu::FindItem(menuWxID));
		return item ? item->GetId() : wxID_NONE;
	}

	void SendOrQueueEvent(KxMenu* menu, KxMenuEvent& event)
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
	void SendOnShowEvent(KxMenu* menu, const wxPoint& pos)
	{
		KxMenuEvent event(KxEVT_MENU_OPEN, menu);
		event.SetPosition(pos);
		event.SetPopup(true);
		menu->ProcessEvent(event);
	}
	void SendOnCloseEvent(KxMenu* menu, const wxPoint& pos)
	{
		KxMenuEvent event(KxEVT_MENU_CLOSE, menu);
		event.SetPosition(pos);
		event.SetPopup(true);
		menu->ProcessEvent(event);
	}
	KxMenuItem* FindItemByEvent(KxMenu* menu, const wxEvent& event, KxMenu** owningMenu = nullptr)
	{
		if (event.GetId() != wxID_NONE)
		{
			return menu->FindItem(event.GetId(), owningMenu);
		}
		return nullptr;
	}

	class ShowMenuScope
	{
		private:
			KxMenu& m_Menu;
			wxWindow* m_Window = nullptr;
			HWND m_Handle = nullptr;

		public:
			ShowMenuScope(KxMenu& menu, wxWindow* window, HWND hWnd)
				:m_Menu(menu), m_Window(window), m_Handle(hWnd)
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

	const DWORD AlignmentMask = TPM_LEFTALIGN|TPM_CENTERALIGN|TPM_RIGHTALIGN|TPM_TOPALIGN|TPM_VCENTERALIGN|TPM_BOTTOMALIGN|TPM_HORIZONTAL|TPM_VERTICAL;
}

wxIMPLEMENT_DYNAMIC_CLASS(KxMenu, wxMenu);

bool KxMenu::EndMenu()
{
	return ::EndMenu();
}
KxMenu* KxMenu::GetCurrentMenu()
{
	return ms_CurrentMenu;
}

void KxMenu::OnSelectItem(wxCommandEvent& event)
{
	if (event.GetId() != wxID_NONE)
	{
		ProcessItemSelection(event.GetId(), &event);
	}
}
void KxMenu::OnHoverItem(wxMenuEvent& event)
{
	KxMenuItem* item = FindItemByEvent(this, event);
	if (item)
	{
		KxMenuEvent itemEvent(KxEVT_MENU_HOVER, item->GetMenu(), item);
		itemEvent.SetEventObject(item->GetEventHandler());
		if (!item->ProcessEvent(itemEvent))
		{
			// If item didn't processed this event send it to menu
			item->GetMenu()->ProcessEvent(itemEvent);
		}
	}
	else
	{
		KxMenuEvent itemEvent(KxEVT_MENU_HOVER, this);
		itemEvent.SetEventObject(this);
		itemEvent.SetId(event.GetId());
		if (KxShellMenu* shellMenu = dynamic_cast<KxShellMenu*>(this))
		{
			itemEvent.SetHelpString(shellMenu->GetHelpString(WxIDToWin(event.GetId())));
		}
		ProcessEvent(itemEvent);
	}
	event.Skip(false);
}

WORD KxMenu::DoShowMenu(wxWindow* window, const wxPoint& showPos, DWORD alignment, bool async)
{
	wxPoint pos = TranslateCoordinates(window, showPos);
	HWND hWnd = GetParentWindowHandle(window);

	ShowMenuScope showMenuScope(*this, window, hWnd);
	SendOnShowEvent(this, pos);

	if (!async)
	{
		ms_CurrentMenu = this;
	}

	int ret = ::TrackPopupMenu(GetHMenu(), (async ? 0 : TPM_RETURNCMD)|TPM_RECURSE|TPM_LEFTBUTTON|(alignment & AlignmentMask), pos.x, pos.y, 0, hWnd, nullptr);
	
	if (!async)
	{
		ms_CurrentMenu = nullptr;
		SendOnCloseEvent(this, pos);
	}
	return ret;
}
void KxMenu::ProcessItemSelection(int menuWxID, wxCommandEvent* event)
{
	bool allowSendEvent = true;
	KxMenuItem* item = dynamic_cast<KxMenuItem*>(FindItem(menuWxID));
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
			KxMenuEvent menuEvent(KxEVT_MENU_SELECT, item->GetMenu(), item);
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
		KxMenuEvent menuEvent(KxEVT_MENU_SELECT, item->GetMenu(), item);
		menuEvent.SetEventObject(this);
		menuEvent.SetItem(item);
		item->GetMenu()->ProcessEvent(menuEvent);
		if (event)
		{
			event->Skip(menuEvent.GetSkipped());
		}
	}
}

KxMenu::KxMenu(const wxString& label, long style)
	:wxMenu(label, style)
{
	Bind(wxEVT_MENU, &KxMenu::OnSelectItem, this);
	Bind(wxEVT_MENU_HIGHLIGHT, &KxMenu::OnHoverItem, this);
}
KxMenu::~KxMenu()
{
}

wxWindowID KxMenu::Show(wxWindow* window, const wxPoint& pos, DWORD alignment)
{
	WORD menuWinID = DoShowMenu(window, pos, alignment, false);
	wxWindowID id = GetSelectedItemID(this, menuWinID);

	if (menuWinID != 0)
	{
		ProcessItemSelection(id);
	}
	return id;
}
wxWindowID KxMenu::ShowAsPopup(wxWindow* window, int offset, DWORD alignment)
{
	wxPoint pos = TranslateCoordinatesAsPopup(window, offset, alignment);
	return Show(window, pos, alignment);
}

void KxMenu::ShowAsync(wxWindow& window, const wxPoint& pos, DWORD alignment)
{
	DoShowMenu(&window, pos, alignment, true);
}
void KxMenu::ShowAsPopupAsync(wxWindow& window, int offset, DWORD alignment)
{
	wxPoint pos = TranslateCoordinatesAsPopup(&window, offset, alignment);
	ShowAsync(window, pos, alignment);
}

KxMenuItem* KxMenu::Add(KxMenuItem* item)
{
	return wxMenu::Append(item) ? item : nullptr;
}
KxMenuItem* KxMenu::Add(KxMenu* subMenu, const wxString& label, const wxString& helpString)
{
	return Add(new KxMenuItem(wxID_ANY, label, helpString, wxITEM_NORMAL, subMenu));
}
KxMenuItem* KxMenu::AddSeparator()
{
	return Add(new KxMenuItem(wxID_SEPARATOR, wxEmptyString, wxEmptyString, wxITEM_SEPARATOR));
}

KxMenuItem* KxMenu::Insert(size_t pos, KxMenuItem* item)
{
	return wxMenu::Insert(pos, item) ? item : nullptr;
}
KxMenuItem* KxMenu::Insert(size_t pos, KxMenu* subMenu, const wxString& label, const wxString& helpString)
{
	return Insert(pos, new KxMenuItem(wxID_ANY, label, helpString, wxITEM_NORMAL, subMenu));
}
KxMenuItem* KxMenu::InsertSeparator(size_t pos)
{
	return Insert(pos, new KxMenuItem(wxID_SEPARATOR, wxEmptyString, wxEmptyString, wxITEM_SEPARATOR));
}

KxMenuItem* KxMenu::Prepend(KxMenuItem* item)
{
	return wxMenu::Prepend(item) ? item : nullptr;
}
KxMenuItem* KxMenu::Prepend(KxMenu* subMenu, const wxString& label, const wxString& helpString)
{
	return Prepend(new KxMenuItem(wxID_ANY, label, helpString, wxITEM_NORMAL, subMenu));
}
KxMenuItem* KxMenu::PrependSeparator()
{
	return Prepend(new KxMenuItem(wxID_SEPARATOR, wxEmptyString, wxEmptyString, wxITEM_SEPARATOR));
}

KxMenuItem* KxMenu::FindChildItem(wxWindowID id, size_t* posPtr) const
{
	return static_cast<KxMenuItem*>(wxMenu::FindChildItem(TranslateItemID(id), posPtr));
}
KxMenuItem* KxMenu::FindItem(wxWindowID id, KxMenu** menu) const
{
	wxMenu* menuWx = nullptr;
	KxMenuItem* item = static_cast<KxMenuItem*>(wxMenu::FindItem(TranslateItemID(id), &menuWx));
	if (menu)
	{
		*menu = static_cast<KxMenu*>(menuWx);
	}
	return item;
}
KxMenuItem* KxMenu::FindItemByPosition(size_t pos) const
{
	return static_cast<KxMenuItem*>(wxMenu::FindItemByPosition(pos));
}

KxMenuItem* KxMenu::RemoveItem(KxMenuItem* item)
{
	return static_cast<KxMenuItem*>(wxMenu::Remove(item));
}
KxMenuItem* KxMenu::RemoveItem(wxWindowID id)
{
	return RemoveItem(FindItem(id));
}
