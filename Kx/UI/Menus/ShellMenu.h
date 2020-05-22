#pragma once
#include "Kx/UI/Common.h"
#include "Menu.h"
#include "Kx/System/COM.h"
#include "Kx/FileSystem/FSPath.h"
struct IContextMenu;
struct IShellFolder;
struct _ITEMIDLIST;

namespace KxFramework::UI
{
	class KX_API ShellMenu: public Menu
	{
		friend class Menu;

		private:
			COMInitGuard m_Initializer;
			COMPtr<IContextMenu> m_ShellMenu;
			COMPtr<IShellFolder> m_ShellFolder;
			COMMemoryPtr<_ITEMIDLIST> m_ShellItemList;

		private:
			bool IsSystemItemID(uint16_t menuWinID) const;
			HResult InvokeCommand(void* windowHandle, uint16_t menuWinID);
			
			String GetString(uint16_t menuWinID, uint32_t index) const;
			String GetCommandString(uint16_t menuWinID) const;
			String GetHelpString(uint16_t menuWinID) const;

		protected:
			uint16_t DoShowMenu(wxWindow* window, const Point& pos, wxAlignment alignment, bool async) override;

		public:
			ShellMenu();
			ShellMenu(const FSPath& path);
			~ShellMenu();

		public:
			bool IsOK() const override
			{
				return m_Initializer && m_ShellMenu && m_ShellFolder && m_ShellItemList;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(ShellMenu);
	};
}
