#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/FlagSet.h"
#include <wx/buffer.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	class DynamicLibrary;
}

namespace kxf::Utility
{
	const wxScopedCharBuffer LoadResource(const DynamicLibrary& library, const String& resourceName, const String& resourceType = wxS("STRING"));
	const wxScopedCharBuffer LoadResource(const DynamicLibrary& library, int resourceID, const String& resourceType = wxS("STRING"));

	FlagSet<intptr_t> GetWindowStyle(void* windowHandle, int index) noexcept;
	FlagSet<intptr_t> SetWindowStyle(void* windowHandle, int index, FlagSet<intptr_t> style) noexcept;
	FlagSet<intptr_t> ModWindowStyle(void* windowHandle, int index, FlagSet<intptr_t> style, bool enable) noexcept;
}
