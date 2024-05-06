#pragma once
#include "KxfPCH.h"
#include "UniChar.h"
#include <Windows.h>
#include <kxf/System/UndefWindows.h>

namespace kxf
{
	UniChar UniChar::ToLowerCase() const noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharLowerW(reinterpret_cast<LPWSTR>(m_Value)));
	}
	UniChar UniChar::ToUpperCase() const noexcept
	{
		#pragma warning(suppress: 4312)
		#pragma warning(suppress: 4302)
		return reinterpret_cast<wchar_t>(::CharUpperW(reinterpret_cast<LPWSTR>(m_Value)));
	}
}
