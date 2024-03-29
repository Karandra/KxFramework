#pragma once
#include "../Common.h"
#include "kxf/General/String.h"
#include "kxf/Localization/Locale.h"
#include "kxf/System/UndefWindows.h"

namespace kxf::System::Private
{
	String FormatMessage(const void* source, uint32_t messageID, uint32_t flags = 0, const Locale& locale = {}) noexcept;

	String ResourceTypeToName(size_t id);
	String ResourceTypeToName(const wchar_t* id);
	const wchar_t* MakeIntResource(int resID);

	FlagSet<uint32_t> MapSystemProcessAccess(FlagSet<SystemProcessAccess> access) noexcept;

	std::optional<uint32_t> MapSystemProcessPriority(SystemProcessPriority priority) noexcept;
	SystemProcessPriority MapSystemProcessPriority(uint32_t priority) noexcept;

	String GetKernelObjectName(const String& name, KernelObjectNamespace ns);
}
