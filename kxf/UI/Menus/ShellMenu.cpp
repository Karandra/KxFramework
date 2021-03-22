#include "KxfPCH.h"
#include "ShellMenu.h"
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <Shldisp.h>
#include <ShlObj.h>

namespace
{
	constexpr int g_MinShellItemID = 0x1;
	constexpr int g_MaxShellItemID = 0x7FFF;
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ShellMenu, Menu);

	ShellMenu::ShellMenu()
		:m_Initializer(COMThreadingModel::Apartment)
	{
	}
	ShellMenu::ShellMenu(const FSPath& path)
		:ShellMenu()
	{
		String pathName = path.GetFullPathWithNS();
		if (HResult hr = ::SHParseDisplayName(pathName.wc_str(), nullptr, &m_ShellItemList, 0, nullptr))
		{
			COMMemoryPtr<const ITEMIDLIST> childID;
			if (hr = ::SHBindToParent(m_ShellItemList, IID_IShellFolder, m_ShellFolder.GetAddress(), &childID))
			{
				wxWindow* window = wxTheApp->GetTopWindow();
				if (hr = m_ShellFolder->GetUIObjectOf(window ? window->GetHandle() : nullptr, 1, &childID, IID_IContextMenu, 0, m_ShellMenu.GetAddress()))
				{
					m_ShellMenu->QueryContextMenu(GetHMenu(), 0, g_MinShellItemID, g_MaxShellItemID, CMF_NORMAL);
				}
			}
		}
	}
	ShellMenu::~ShellMenu()
	{
	}

	bool ShellMenu::IsSystemItemID(uint16_t menuWinID) const
	{
		return menuWinID >= g_MinShellItemID && menuWinID <= g_MaxShellItemID;
	}
	HResult ShellMenu::InvokeCommand(void* windowHandle, uint16_t menuWinID)
	{
		CMINVOKECOMMANDINFOEX info = {};
		info.cbSize = sizeof(info);
		info.fMask = CMIC_MASK_UNICODE;
		info.hwnd = reinterpret_cast<HWND>(windowHandle);
		info.lpVerb = MAKEINTRESOURCEA(menuWinID - 1);
		info.lpVerbW = MAKEINTRESOURCEW(menuWinID - 1);
		info.nShow = SW_SHOWNORMAL;

		return m_ShellMenu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&info));
	}

	String ShellMenu::GetString(uint16_t menuWinID, uint32_t index) const
	{
		if (menuWinID != 0)
		{;
			wchar_t buffer[4096] = {};
			m_ShellMenu->GetCommandString(static_cast<UINT_PTR>(menuWinID - 1), index, nullptr, reinterpret_cast<char*>(buffer), std::size(buffer) - sizeof(wchar_t));
			return buffer;
		}
		return {};
	}
	String ShellMenu::GetCommandString(uint16_t menuWinID) const
	{
		return GetString(menuWinID, GCS_VERBW);
	}
	String ShellMenu::GetHelpString(uint16_t menuWinID) const
	{
		return GetString(menuWinID, GCS_HELPTEXTW);
	}

	uint16_t ShellMenu::DoShowMenu(wxWindow* window, const Point& pos, FlagSet<Alignment> alignment, bool async)
	{
		// Async is not supported here
		uint16_t winID = Menu::DoShowMenu(window, pos, alignment, false);
		if (winID != 0)
		{
			MenuEvent event(MenuEvent::EvtSelect, this);
			event.SetEventObject(this);
			event.SetId(WinMenuRetToWx(winID));
			event.SetInt(IsSystemItemID(winID));
			event.SetString(GetCommandString(winID));
			event.SetHelpString(GetHelpString(winID));
			event.SetPosition(pos);
			event.SetPopup(true);
			bool isProcessed = SafelyProcessEvent(event);

			// If selection wasn't processed or skipped invoke shell command
			if (!isProcessed || event.GetSkipped())
			{
				InvokeCommand(window ? window->GetHandle() : nullptr, winID);
			}
		}
		return winID;
	}
}
