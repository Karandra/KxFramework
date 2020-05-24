#pragma once
#include "Common.h"
#include "SystemProcess/ISystemProcess.h"
#include "SystemProcess/ISystemProcessStdIO.h"
#include "SystemProcess/SystemProcessInfo.h"
#include "SystemProcess/ProcessEvent.h"

namespace kxf::System
{
	size_t EnumRunningProcesses(std::function<bool(uint32_t)> func);

	std::unique_ptr<ISystemProcess> CreateProcess(const ISystemProcess& info, wxEvtHandler* evtHandler = nullptr, FlagSet<CreateSystemProcessFlag> flags = {});
}
