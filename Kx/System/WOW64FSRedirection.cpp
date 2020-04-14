#include "KxStdAfx.h"
#include "WOW64FSRedirection.h"
#include "NativeAPI.h"

namespace KxFramework
{
	DisableWOW64FSRedirection::DisableWOW64FSRedirection() noexcept
	{
		if (NativeAPI::Kernel32::Wow64DisableWow64FsRedirection)
		{
			NativeAPI::Kernel32::Wow64DisableWow64FsRedirection(&m_Value);
		}
	}
	DisableWOW64FSRedirection::~DisableWOW64FSRedirection() noexcept
	{
		if (NativeAPI::Kernel32::Wow64RevertWow64FsRedirection && m_Value)
		{
			NativeAPI::Kernel32::Wow64RevertWow64FsRedirection(&m_Value);
		}
	}
}
