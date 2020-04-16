#include "KxStdAfx.h"
#include "ShellLink.h"
#include "Private/Shell.h"
#include "Kx/Utility/Common.h"
#include <ShObjIDL.h>
#include "UndefWindows.h"

namespace KxFramework
{
	ShellLink::ShellLink() noexcept
	{
		COMPtr<IShellLinkW> shellLink;
		if (HResult(::CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, shellLink.GetAddress())))
		{
			m_ShellLink = std::move(shellLink);
		}
	}
	ShellLink::~ShellLink() noexcept
	{
	}

	HResult ShellLink::Load(const FSPath& path)
	{
		HResult hr = E_INVALIDARG;

		if (path)
		{
			COMPtr<IPersistFile> persistFile;
			if (hr = m_ShellLink->QueryInterface(&persistFile))
			{
				String pathString = path.GetFullPathWithNS();
				return persistFile->Load(pathString.wc_str(), 0);
			}
		}
		return hr;
	}
	HResult ShellLink::Save(const FSPath& path) const
	{
		HResult hr = E_INVALIDARG;

		if (path)
		{
			COMPtr<IPersistFile> persistFile;
			if (hr = m_ShellLink->QueryInterface(&persistFile))
			{
				String pathString = path.GetFullPathWithNS();
				return persistFile->Save(pathString.wc_str(), TRUE);
			}
		}
		return hr;
	}

	String ShellLink::GetTarget() const
	{
		wxChar buffer[INT16_MAX] = {};
		WIN32_FIND_DATAW findData = {};
		if (HResult(m_ShellLink->GetPath(buffer, std::size(buffer), &findData, 0)))
		{
			return buffer;
		}
	}
	HResult ShellLink::SetTarget(const FSPath& path)
	{
		String pathString = path.GetFullPathWithNS();
		return m_ShellLink->SetPath(pathString.wc_str());
	}

	String ShellLink::GetArguments() const
	{
		wxChar buffer[INT16_MAX] = {};
		if (HResult(m_ShellLink->GetArguments(buffer, std::size(buffer))))
		{
			return buffer;
		}
		return {};
	}
	HResult ShellLink::SetArguments(const String& value)
	{
		return m_ShellLink->SetArguments(value.wc_str());
	}

	String ShellLink::GetWorkingDirectory() const
	{
		wxChar buffer[INT16_MAX] = {};
		if (HResult(m_ShellLink->GetWorkingDirectory(buffer, std::size(buffer))))
		{
			return buffer;
		}
		return {};
	}
	HResult ShellLink::SetWorkingDirectory(const FSPath& path)
	{
		String pathString = path.GetFullPathWithNS();
		return m_ShellLink->SetWorkingDirectory(pathString.wc_str());
	}

	String ShellLink::GetDescription() const
	{
		wxChar buffer[INFOTIPSIZE] = {};
		if (HResult(m_ShellLink->GetDescription(buffer, std::size(buffer))))
		{
			return buffer;
		}
		return {};
	}
	HResult ShellLink::SetDescription(const String& value)
	{
		return m_ShellLink->SetDescription(value.wc_str());
	}

	FSPath ShellLink::GetIconLocation() const
	{
		int index = 0;
		int length = 0;

		if (HResult(m_ShellLink->GetIconLocation(nullptr, length, &index)))
		{
			String path;
			if (HResult(m_ShellLink->GetIconLocation(wxStringBuffer(path, length), length, &index)))
			{
				return path;
			}
		}
		return {};
	}
	HResult ShellLink::SetIconLocation(const FSPath& path, int index)
	{
		String pathString = path.GetFullPathWithNS();
		return m_ShellLink->SetIconLocation(pathString.wc_str(), index);
	}
	std::optional<int> ShellLink::GetIconIndex() const noexcept
	{
		int index = -1;
		if (HResult(m_ShellLink->GetIconLocation(nullptr, 0, &index)))
		{
			return index;
		}
		return {};
	}

	WindowVisibilityOption ShellLink::GetShowCommand() const noexcept
	{
		int command = 0;
		if (HResult(m_ShellLink->GetShowCmd(&command)))
		{
			return Shell::Private::MapWindowVisibilityOption(command);
		}
		return WindowVisibilityOption::None;
	}
	HResult ShellLink::SetShowCommand(WindowVisibilityOption command) noexcept
	{
		return m_ShellLink->SetShowCmd(Shell::Private::MapWindowVisibilityOption(command).value_or(SW_SHOWNORMAL));
	}

	wxKeyEvent ShellLink::GetHotKey() const noexcept
	{
		wxKeyEvent keyState;

		WORD hotKeys = 0;
		if (HResult(m_ShellLink->GetHotkey(&hotKeys)))
		{
			keyState.m_keyCode = Utility::IntLowPart<BYTE>(hotKeys);

			BYTE modifiers = Utility::IntHighPart<BYTE>(hotKeys);
			keyState.m_controlDown = modifiers & HOTKEYF_CONTROL;
			keyState.m_altDown = modifiers & HOTKEYF_ALT;
			keyState.m_shiftDown = modifiers & HOTKEYF_SHIFT;
		}
		return keyState;
	}
	HResult ShellLink::SetHotKey(const wxKeyEvent& keyState) noexcept
	{
		BYTE modifiers = 0;
		Utility::AddFlagRef(modifiers, HOTKEYF_CONTROL, keyState.ControlDown());
		Utility::AddFlagRef(modifiers, HOTKEYF_ALT, keyState.AltDown());
		Utility::AddFlagRef(modifiers, HOTKEYF_SHIFT, keyState.ShiftDown());

		return m_ShellLink->SetHotkey(Utility::IntFromLowHigh<WORD>(modifiers, static_cast<BYTE>(keyState.GetKeyCode())));
	}
}
