#include "KxStdAfx.h"
#include "KxFramework/KxShellMenu.h"
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <Shldisp.h>
#include <ShlObj.h>

wxIMPLEMENT_DYNAMIC_CLASS(KxShellMenu, KxMenu);

KxShellMenu::KxShellMenu()
	:m_Initializer(COINIT_APARTMENTTHREADED)
{
}
KxShellMenu::KxShellMenu(const wxString& path)
	:KxShellMenu()
{
	HRESULT res = SHParseDisplayName(path, 0, &m_ShellItemList, 0, 0);
	if (SUCCEEDED(res) && m_ShellItemList)
	{
		LPCITEMIDLIST childID = NULL;
		res = SHBindToParent(m_ShellItemList, IID_IShellFolder, m_ShellFolder.GetPVoid(), &childID);
		if (SUCCEEDED(res) && m_ShellFolder)
		{
			wxWindow* window = wxTheApp->GetTopWindow();
			res = m_ShellFolder->GetUIObjectOf(window ? window->GetHandle() : NULL, 1, &childID, IID_IContextMenu, 0, m_ShellMenu.GetPVoid());
			if (SUCCEEDED(res) && m_ShellMenu)
			{
				m_ShellMenu->QueryContextMenu(GetHMenu(), 0, MinShellItemID, MaxShellItemID, CMF_NORMAL);
			}
		}
	}
}
KxShellMenu::~KxShellMenu()
{
	if (m_ShellItemList)
	{
		CoTaskMemFree(m_ShellItemList);
	}
}

bool KxShellMenu::IsSystemItemID(WORD menuWinID) const
{
	return menuWinID >= MinShellItemID || menuWinID <= MaxShellItemID;
}
void KxShellMenu::InvokeCommand(HWND hWnd, WORD menuWinID)
{
	CMINVOKECOMMANDINFOEX info = {0};
	info.cbSize = sizeof(info);
	info.fMask = CMIC_MASK_UNICODE;
	info.hwnd = hWnd;
	info.lpVerb = MAKEINTRESOURCEA(menuWinID - 1);
	info.lpVerbW = MAKEINTRESOURCEW(menuWinID - 1);
	info.nShow = SW_SHOWNORMAL;
	m_ShellMenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
}

wxString KxShellMenu::GetString(WORD menuWinID, DWORD index) const
{
	if (menuWinID != 0)
	{
		const size_t length = 4096;
		WCHAR buffer[length] = {0};

		m_ShellMenu->GetCommandString((UINT_PTR)(menuWinID - 1), index, NULL, (CHAR*)buffer, length - sizeof(WCHAR));
		return buffer;
	}
	return wxEmptyString;
}
wxString KxShellMenu::GetCommandString(WORD menuWinID) const
{
	return GetString(menuWinID, GCS_VERBW);
}
wxString KxShellMenu::GetHelpString(WORD menuWinID) const
{
	return GetString(menuWinID, GCS_HELPTEXTW);
}

wxWindowID KxShellMenu::Show(wxWindow* window, const wxPoint& pos, DWORD alignment)
{
	WORD menuWinID = ShowNoEvent(window, pos, alignment);
	if (menuWinID != 0)
	{
		KxMenuEvent menuEvent(KxEVT_MENU_SELECT, this);
		menuEvent.SetEventObject(this);
		menuEvent.SetId(WinMenuRetToWx(menuWinID));
		menuEvent.SetInt(IsSystemItemID(menuWinID));
		menuEvent.SetString(GetCommandString(menuWinID));
		menuEvent.SetHelpString(GetHelpString(menuWinID));
		menuEvent.SetPosition(pos);
		menuEvent.SetPopup(true);
		bool bProcessed = SafelyProcessEvent(menuEvent);

		// If selection wasn't processed or skipped invoke shell command
		if (!bProcessed || menuEvent.GetSkipped())
		{
			InvokeCommand(window ? window->GetHandle() : NULL, menuWinID);
		}
	}
	return WinMenuRetToWx(menuWinID);
}
