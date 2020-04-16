#pragma once
#include "Kx/Common.hpp"

namespace KxFramework
{
	enum class ErrorCodeCategory
	{
		Unknown = -1,

		Generic,
		Win32,
		HResult,
		NtStatus
	};
	enum class SHWindowCommand: uint32_t
	{
		None = 0,

		// Command
		Show = 1 << 0,
		Hide = 1 << 1,
		Minimize = 1 << 2,
		Maximize = 1 << 3,
		Restore = 1 << 4,

		// Modifiers
		Force = 1 << 16,
		Default = 1 << 17,
		Inactive = 1 << 18,
	};
	enum class MemoryProtection: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Execute = 1 << 2,

		RW = Read|Write,
		RX = Read|Execute,
		RWX = Read|Write|Execute,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(SHWindowCommand);
		Kx_EnumClass_AllowEverything(MemoryProtection);
	}
}
