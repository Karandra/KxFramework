#include "stdafx.h"
#include "SystemProcessInfo.h"
#include <Windows.h>
#include <PsAPI.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	SystemProcessInfo SystemProcessInfo::GetCurrentProcess()
	{
		return SystemProcessInfo(::GetCurrentProcessId());
	}

	bool SystemProcessInfo::IsCurrent() const
	{
		return m_PID == ::GetCurrentProcessId();
	}
}
