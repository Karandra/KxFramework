#include "KxfPCH.h"
#include "System.h"
#include "Common.h"

namespace kxf::Utility
{
	FlagSet<intptr_t> GetWindowStyle(void* windowHandle, int index) noexcept
	{
		return ::GetWindowLongPtrW(reinterpret_cast<HWND>(windowHandle), index);
	}
	FlagSet<intptr_t> SetWindowStyle(void* windowHandle, int index, FlagSet<intptr_t> style) noexcept
	{
		return ::SetWindowLongPtrW(reinterpret_cast<HWND>(windowHandle), index, style.GetValue());
	}
	FlagSet<intptr_t> ModWindowStyle(void* windowHandle, int index, FlagSet<intptr_t> style, bool enable) noexcept
	{
		return SetWindowStyle(windowHandle, index, GetWindowStyle(windowHandle, index).Mod(style, enable));
	}
}
