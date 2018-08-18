#include "KxStdAfx.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxMenuEvent.h"
#include "KxFramework/KxShellMenu.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxMenu, wxMenu);

KxMenu* KxMenu::m_CurrentMenu = NULL;

bool KxMenu::EndMenu()
{
	return ::EndMenu();
}
KxMenu* KxMenu::GetCurrentMenu()
{
	return m_CurrentMenu;
}

KxMenuItem* KxMenu::FindItemByEvent(const wxEvent& event, KxMenu** menu)
{
	if (event.GetId() != wxID_NONE)
	{
		return FindItem(event.GetId(), menu);
	}
	return NULL;
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
	KxMenuItem* item = FindItemByEvent(event);
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
		if (KxShellMenu* pShellMenu = dynamic_cast<KxShellMenu*>(this))
		{
			itemEvent.SetHelpString(pShellMenu->GetHelpString(WxIDToWin(event.GetId())));
		}
		ProcessEvent(itemEvent);
	}
	event.Skip(false);
}

void KxMenu::wxYieldForCommandsOnly()
{
	// This right from wxWindowMSW::DoPopupMenu

	// peek all WM_COMMANDs (it will always return WM_QUIT too but we don't
	// want to process it here)
	MSG msg;
	while (::PeekMessageW(&msg, (HWND)NULL, WM_COMMAND, WM_COMMAND, PM_REMOVE))
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
WORD KxMenu::ShowNoEvent(wxWindow* window, const wxPoint& showPos, DWORD alignment)
{
	wxPoint pos = showPos;

	// Check position
	if (pos == wxDefaultPosition)
	{
		pos = wxGetMousePosition();
	}
	else
	{
		if (window)
		{
			pos = window->ClientToScreen(pos);
		}
	}

	HWND hWnd = NULL;
	if (window)
	{
		hWnd = window->GetHandle();
	}
	else
	{
		if (wxTheApp->GetTopWindow())
		{
			window = wxTheApp->GetTopWindow();
			hWnd = window->GetHandle();
		}
	}
	if (window)
	{
		SetInvokingWindow(window);
		SetForegroundWindow(hWnd);
	}
	UpdateUI();

	KxMenuEvent onShowEvent(KxEVT_MENU_OPEN, this);
	onShowEvent.SetPosition(pos);
	onShowEvent.SetPopup(true);
	ProcessEvent(onShowEvent);

	m_CurrentMenu = this;
	int ret = ::TrackPopupMenu(GetHMenu(), TPM_RETURNCMD|TPM_RECURSE|TPM_LEFTBUTTON|(alignment & AlignmentMask), pos.x, pos.y, 0, hWnd, NULL);
	m_CurrentMenu = NULL;

	KxMenuEvent onCloseEvent(KxEVT_MENU_CLOSE, this);
	onCloseEvent.SetPosition(pos);
	onCloseEvent.SetPopup(true);
	ProcessEvent(onCloseEvent);

	if (hWnd)
	{
		PostMessageW(hWnd, WM_NULL, 0, 0);
	}
	if (wxThread::IsMain())
	{
		wxYieldForCommandsOnly();
	}

	if (window)
	{
		SetInvokingWindow(NULL);
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
	if (allowSendEvent)
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
	WORD menuWinID = ShowNoEvent(window, pos, alignment);
	if (menuWinID != 0)
	{
		ProcessItemSelection(menuWinID);
	}

	wxWindowID menuWxID = WinMenuRetToWx(menuWinID);
	KxMenuItem* item = dynamic_cast<KxMenuItem*>(FindItem(menuWxID));
	return item ? item->GetId() : menuWxID;
}
wxWindowID KxMenu::ShowAsPopup(wxWindow* window, int offset, DWORD alignment)
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
	return Show(window, pos, alignment);
}

KxMenuItem* KxMenu::Add(KxMenuItem* item)
{
	return wxMenu::Append(item) ? item : NULL;
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
	return wxMenu::Insert(pos, item) ? item : NULL;
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
	return wxMenu::Prepend(item) ? item : NULL;
}
KxMenuItem* KxMenu::Prepend(KxMenu* subMenu, const wxString& label, const wxString& helpString)
{
	return Prepend(new KxMenuItem(wxID_ANY, label, helpString, wxITEM_NORMAL, subMenu));
}
KxMenuItem* KxMenu::PrependSeparator()
{
	return Prepend(new KxMenuItem(wxID_SEPARATOR, wxEmptyString, wxEmptyString, wxITEM_SEPARATOR));
}

KxMenuItem* KxMenu::FindChildItem(wxWindowID id, size_t* nPosPtr) const
{
	return static_cast<KxMenuItem*>(wxMenu::FindChildItem(id, nPosPtr));
}
KxMenuItem* KxMenu::FindItem(wxWindowID id, KxMenu** menu) const
{
	wxMenu* pWxMenu = NULL;
	KxMenuItem* item = static_cast<KxMenuItem*>(wxMenu::FindItem(id, &pWxMenu));
	if (menu)
	{
		*menu = static_cast<KxMenu*>(pWxMenu);
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
