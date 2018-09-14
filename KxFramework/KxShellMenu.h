#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxCOM.h"
#include "KxFramework/KxMenu.h"
#include <shobjidl.h>

class KxShellMenu: public KxMenu
{
	friend class KxMenu;

	private:
		static const int MinShellItemID = 0x1;
		static const int MaxShellItemID = 0x7FFF;
	
	private:
		KxCOMInit m_Initializer;

		KxCOMPtr<IContextMenu> m_ShellMenu;
		KxCOMPtr<IShellFolder> m_ShellFolder;
		ITEMIDLIST* m_ShellItemList = NULL;

	private:
		bool IsSystemItemID(WORD menuWinID) const;
		void InvokeCommand(HWND hWnd, WORD menuWinID);
		
		wxString GetString(WORD menuWinID, DWORD index) const;
		wxString GetCommandString(WORD menuWinID) const;
		wxString GetHelpString(WORD menuWinID) const;

	public:
		KxShellMenu();
		KxShellMenu(const wxString& path);
		virtual ~KxShellMenu();

	public:
		virtual bool IsOK() const override
		{
			return m_Initializer && m_ShellMenu && m_ShellFolder && m_ShellItemList;
		}
		virtual wxWindowID Show(wxWindow* window = NULL, const wxPoint& pos = wxDefaultPosition, DWORD alignment = DefaultAlignment) override;

	public:
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxShellMenu);
};