#pragma once
#include "Common.h"
#include "SystemProcess/ISystemProcess.h"
#include "SystemProcess/ISystemProcessStdIO.h"
#include "SystemProcess/SystemProcessInfo.h"
#include "SystemProcess/ProcessEvent.h"
#include "kxf/EventSystem/EvtHandlerDelegate.h"

namespace kxf::System
{
	KX_API size_t EnumRunningProcesses(std::function<bool(uint32_t)> func);

	KX_API std::unique_ptr<ISystemProcess> CreateProcess(const ISystemProcess& info, EvtHandlerDelegate evtHandler = {}, FlagSet<CreateSystemProcessFlag> flags = {});
}
