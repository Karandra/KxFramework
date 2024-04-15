#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"
#include "kxf/Core/FlagSet.h"
#include "kxf/System/UndefWindows.h"

namespace kxf::Utility
{
	KX_API FlagSet<intptr_t> GetWindowStyle(void* windowHandle, int index) noexcept;
	KX_API FlagSet<intptr_t> SetWindowStyle(void* windowHandle, int index, FlagSet<intptr_t> style) noexcept;
	KX_API FlagSet<intptr_t> ModWindowStyle(void* windowHandle, int index, FlagSet<intptr_t> style, bool enable) noexcept;
}
