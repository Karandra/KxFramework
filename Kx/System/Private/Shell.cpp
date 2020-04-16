#include "KxStdAfx.h"
#include "Shell.h"
#include <WinUser.h>

namespace KxFramework::Shell::Private
{
	std::optional<uint32_t> MapWindowVisibilityOption(WindowVisibilityOption option) noexcept
	{
		// Show
		if (option & WindowVisibilityOption::Show)
		{
			if (option & WindowVisibilityOption::Default)
			{
				return SW_SHOWDEFAULT;
			}
			else if (option & WindowVisibilityOption::Minimize)
			{
				if (option & WindowVisibilityOption::Inactive)
				{
					return SW_SHOWMINNOACTIVE;
				}
				return SW_SHOWMINIMIZED;
			}
			else if (option & WindowVisibilityOption::Maximize)
			{
				return SW_SHOWMAXIMIZED;
			}
			else if (option & WindowVisibilityOption::Inactive)
			{
				// Or should I use 'SW_SHOWNA'? I don't really see the difference.
				return SW_SHOWNOACTIVATE;
			}
			return SW_SHOWNORMAL;
		}

		// Hide
		if (option & WindowVisibilityOption::Hide)
		{
			return SW_HIDE;
		}

		// Maximize
		if (option & WindowVisibilityOption::Minimize)
		{
			if (option & WindowVisibilityOption::Force)
			{
				return SW_FORCEMINIMIZE;
			}
			return SW_MINIMIZE;
		}

		// Maximize
		if (option & WindowVisibilityOption::Maximize)
		{
			return SW_MAXIMIZE;
		}

		// Restore
		if (option & WindowVisibilityOption::Restore)
		{
			return SW_RESTORE;
		}
		return {};
	}
	WindowVisibilityOption MapWindowVisibilityOption(uint32_t option) noexcept
	{
		switch (option)
		{
			case SW_SHOWNORMAL:
			{
				return WindowVisibilityOption::Show;
			}
			case SW_SHOWDEFAULT:
			{
				return WindowVisibilityOption::Show|WindowVisibilityOption::Default;
			}
			case SW_SHOWMINIMIZED:
			{
				return WindowVisibilityOption::Show|WindowVisibilityOption::Minimize;
			}
			case SW_SHOWMINNOACTIVE:
			{
				return WindowVisibilityOption::Show|WindowVisibilityOption::Minimize|WindowVisibilityOption::Inactive;
			}
			case SW_SHOWMAXIMIZED:
			{
				return WindowVisibilityOption::Show|WindowVisibilityOption::Maximize;
			}
			case SW_SHOWNA:
			case SW_SHOWNOACTIVATE:
			{
				return WindowVisibilityOption::Show|WindowVisibilityOption::Inactive;
			}
			case SW_HIDE:
			{
				return WindowVisibilityOption::Hide;
			}
			case SW_MINIMIZE:
			{
				return WindowVisibilityOption::Minimize;
			}
			case SW_FORCEMINIMIZE:
			{
				return WindowVisibilityOption::Minimize|WindowVisibilityOption::Force;
			}
			case SW_RESTORE:
			{
				return WindowVisibilityOption::Restore;
			}
		}
		return WindowVisibilityOption::None;
	}
}
