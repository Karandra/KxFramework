#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxMenu.h"
#include <shobjidl.h>

class KxShellMenu: public KxMenu
{
	friend class KxMenu;

	private:
		static const int MinShellItemID = 0x1;
		static const int MaxShellItemID = 0x7FFF;
	
	public:
		static KxShellMenu* CreateMenuForFileSystemObject(const wxString& path);

	private:
		IContextMenu* m_ShellMenu = NULL;
		IShellFolder* m_ShellFolder = NULL;
		ITEMIDLIST* m_ShellItemList = NULL;

	private:
		bool IsSystemItemID(WORD menuWinID) const;
		void InvokeCommand(HWND hWnd, WORD menuWinID);
		wxString GetString(WORD menuWinID, DWORD index) const;
		wxString GetCommandString(WORD menuWinID) const;
		wxString GetHelpString(WORD menuWinID) const;

	public:
		KxShellMenu() {}
		KxShellMenu(IContextMenu* pShellMenu, IShellFolder* pShellFolder, ITEMIDLIST* pShellItemList);

	public:
		virtual ~KxShellMenu();
		virtual wxWindowID Show(wxWindow* window = NULL, const wxPoint& pos = wxDefaultPosition, DWORD alignment = DefaultAlignment) override;

		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxShellMenu);
};