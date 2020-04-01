#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxMenu.h"
#include "Kx/System/COM.h"
#include <shobjidl.h>

class KX_API KxShellMenu: public KxMenu
{
	friend class KxMenu;

	private:
		KxFramework::COMInitGuard m_Initializer;

		KxFramework::COMPtr<IContextMenu> m_ShellMenu;
		KxFramework::COMPtr<IShellFolder> m_ShellFolder;
		ITEMIDLIST* m_ShellItemList = nullptr;

	private:
		bool IsSystemItemID(WORD menuWinID) const;
		void InvokeCommand(HWND hWnd, WORD menuWinID);
		
		wxString GetString(WORD menuWinID, DWORD index) const;
		wxString GetCommandString(WORD menuWinID) const;
		wxString GetHelpString(WORD menuWinID) const;

	protected:
		virtual WORD DoShowMenu(wxWindow* window, const wxPoint& pos, DWORD alignment, bool async) override;

	public:
		KxShellMenu();
		KxShellMenu(const wxString& path);
		virtual ~KxShellMenu();

	public:
		bool IsOK() const override
		{
			return m_Initializer && m_ShellMenu && m_ShellFolder && m_ShellItemList;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxShellMenu);
};