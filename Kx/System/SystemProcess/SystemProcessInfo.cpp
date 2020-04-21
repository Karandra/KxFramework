#include "stdafx.h"
#include "SystemProcessInfo.h"
#include <PsAPI.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework
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

