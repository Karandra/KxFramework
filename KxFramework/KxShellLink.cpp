/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxShellLink.h"
#include "KxFramework/KxUtility.h"
#include <ShObjIDL.h>

KxShellLink::KxShellLink(const wxString& filePath)
{
	HRESULT res = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&m_Instance);
	if (SUCCEEDED(res))
	{
		if (!filePath.IsEmpty())
		{
			IPersistFile* persistFile = nullptr;
			if (SUCCEEDED(m_Instance->QueryInterface(IID_IPersistFile, (void**)&persistFile)) && persistFile != nullptr)
			{
				persistFile->Load(filePath.wc_str(), 0);
				persistFile->Release();
			}
		}
	}
	else
	{
		m_Instance = nullptr;
	}
}
KxShellLink::~KxShellLink()
{
	if (m_Instance)
	{
		m_Instance->Release();
	}
}

bool KxShellLink::IsOK() const
{
	return m_Instance != nullptr;
}
bool KxShellLink::Save(const wxString& path) const
{
	if (!path.IsEmpty())
	{
		IPersistFile* persistFile = nullptr;
		if (SUCCEEDED(m_Instance->QueryInterface(IID_IPersistFile, (void**)&persistFile)) && persistFile != nullptr)
		{
			HRESULT res = persistFile->Save(path.wc_str(), TRUE);
			persistFile->Release();
			return SUCCEEDED(res);
		}
	}
	return false;
}

wxString KxShellLink::GetTarget() const
{
	wxString out;
	m_Instance->GetPath(wxStringBuffer(out, MAX_PATH), MAX_PATH, nullptr, 0);
	return out;
}
void KxShellLink::SetTarget(const wxString& value)
{
	m_Instance->SetPath(value.wc_str());
}

wxString KxShellLink::GetArguments() const
{
	wxString out;
	m_Instance->GetArguments(wxStringBuffer(out, INT16_MAX), INT16_MAX);
	out.Shrink();
	return out;
}
void KxShellLink::SetArguments(const wxString& value)
{
	m_Instance->SetArguments(value.wc_str());
}

wxString KxShellLink::GetWorkingFolder() const
{
	wxString out;
	m_Instance->GetWorkingDirectory(wxStringBuffer(out, INT16_MAX), INT16_MAX);
	out.Shrink();
	return out;
}
void KxShellLink::SetWorkingFolder(const wxString& value)
{
	m_Instance->SetWorkingDirectory(value.wc_str());
}

wxString KxShellLink::GetDescription() const
{
	wxString out;
	m_Instance->GetDescription(wxStringBuffer(out, INFOTIPSIZE), INFOTIPSIZE);
	return out;
}
void KxShellLink::SetDescription(const wxString& value)
{
	m_Instance->SetDescription(value.wc_str());
}

wxString KxShellLink::GetIconLocation(int* indexOut) const
{
	int index = 0;
	wxString out;

	m_Instance->GetIconLocation(wxStringBuffer(out, INT16_MAX), INT16_MAX, &index);
	out.Shrink();
	KxUtility::SetIfNotNull(indexOut, index);
	return out;
}
void KxShellLink::SetIconLocation(const wxString& value, int index)
{
	m_Instance->SetIconLocation(value.wc_str(), index);
}
int KxShellLink::GetIconIndex() const
{
	int index = -1;
	m_Instance->GetIconLocation(nullptr, 0, &index);
	return index;
}

KxShellLink::ShowCommand KxShellLink::GetShowCommand() const
{
	int command = KxSHL_SHOW_INVALID_COMMAND;
	if (SUCCEEDED(m_Instance->GetShowCmd(&command)))
	{
		return static_cast<ShowCommand>(command);
	}
	return KxSHL_SHOW_INVALID_COMMAND;
}
void KxShellLink::SetShowCommand(ShowCommand command)
{
	if (command != KxSHL_SHOW_INVALID_COMMAND)
	{
		m_Instance->SetShowCmd(command);
	}
}

wxKeyEvent KxShellLink::GetHotKey() const
{
	wxKeyEvent keyState;

	WORD hotKeys = 0;
	if (SUCCEEDED(m_Instance->GetHotkey(&hotKeys)))
	{
		keyState.m_keyCode = KxUtility::GetIntLowPart<BYTE>(hotKeys);//LOBYTE(nHotKeys);

		BYTE modifiers = KxUtility::GetIntHighPart<BYTE>(hotKeys);//HIBYTE(nHotKeys);
		keyState.m_controlDown = modifiers & HOTKEYF_CONTROL;
		keyState.m_altDown = modifiers & HOTKEYF_ALT;
		keyState.m_shiftDown = modifiers & HOTKEYF_SHIFT;
	}
	return keyState;
}
void KxShellLink::SetHotKey(const wxKeyEvent& keyState)
{
	BYTE modifiers = 0;
	if (keyState.ControlDown())
	{
		modifiers |= HOTKEYF_CONTROL;
	}
	if (keyState.AltDown())
	{
		modifiers |= HOTKEYF_ALT;
	}
	if (keyState.ShiftDown())
	{
		modifiers |= HOTKEYF_SHIFT;
	}
	m_Instance->SetHotkey(KxUtility::MakeInt<WORD>((BYTE)keyState.GetKeyCode(), modifiers));
}
