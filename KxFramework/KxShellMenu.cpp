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

KxShellMenu* KxShellMenu::CreateMenuForFileSystemObject(const wxString& path)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	ITEMIDLIST* pIDs = NULL;
	HRESULT res = SHParseDisplayName(path, 0, &pIDs, 0, 0);
	if (SUCCEEDED(res) && pIDs)
	{
		IShellFolder* pFolder = NULL;
		LPCITEMIDLIST pChildID = NULL;
		res = SHBindToParent(pIDs, IID_IShellFolder, (void**)&pFolder, &pChildID);
		if (SUCCEEDED(res) && pFolder)
		{
			IContextMenu* pContextMenu = NULL;
			wxWindow* window = wxTheApp->GetTopWindow();
			res = pFolder->GetUIObjectOf(window ? window->GetHandle() : NULL, 1, &pChildID, IID_IContextMenu, 0, (void**)&pContextMenu);
			if (SUCCEEDED(res) && pContextMenu)
			{
				return new KxShellMenu(pContextMenu, pFolder, pIDs);
			}
		}
	}
	CoUninitialize();
	return NULL;
}

KxShellMenu::KxShellMenu(IContextMenu* pShellMenu, IShellFolder* pShellFolder, ITEMIDLIST* pShellItemList)
	:m_ShellMenu(pShellMenu), m_ShellFolder(pShellFolder), m_ShellItemList(pShellItemList)
{
	if (pShellMenu && pShellFolder)
	{
		pShellMenu->QueryContextMenu(GetHMenu(), 0, MinShellItemID, MaxShellItemID, CMF_NORMAL);
	}
}
KxShellMenu::~KxShellMenu()
{
	m_ShellMenu->Release();
	m_ShellFolder->Release();
	CoTaskMemFree(m_ShellItemList);
	CoUninitialize();
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
		KxMenuEvent tMenuEvent(KxEVT_MENU_SELECT, this);
		tMenuEvent.SetEventObject(this);
		tMenuEvent.SetId(WinMenuRetToWx(menuWinID));
		tMenuEvent.SetInt(IsSystemItemID(menuWinID));
		tMenuEvent.SetString(GetCommandString(menuWinID));
		tMenuEvent.SetHelpString(GetHelpString(menuWinID));
		tMenuEvent.SetPosition(pos);
		tMenuEvent.SetPopup(true);
		bool bProcessed = SafelyProcessEvent(tMenuEvent);

		// If selection wasn't processed or skipped invoke shell command
		if (!bProcessed || tMenuEvent.GetSkipped())
		{
			InvokeCommand(window ? window->GetHandle() : NULL, menuWinID);
		}
	}
	return WinMenuRetToWx(menuWinID);
}
