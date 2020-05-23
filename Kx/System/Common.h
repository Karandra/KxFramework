#pragma once
#include "Kx/Common.hpp"
#include "Kx/General/String.h"

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
	enum class UserNameFormat
	{
		None = -1,

		Display,
		UniqueID,
		DownLevel,
		Canonical,
		CanonicalEx,
		UserPrincipal,
		ServicePrincipal,
		FullyQualifiedDN
	};

	enum class SystemProcessAccess
	{
		None = 0,

		CreateProcess = 1 << 0,
		CreateThread = 1 << 1,
		QueryInformation = 1 << 2,
		QueryLimitedInformation = 1 << 3,
		SetInformation = 1 << 4,
		SuspendResume = 1 << 5,
		Synchronize = 1 << 6,
		Terminate = 1 << 7,
		VMOperation = 1 << 8,
		VMRead = 1 << 9,
		VMWrite = 1 << 10,

		Everything = CreateProcess|CreateThread|QueryInformation|QueryLimitedInformation|SetInformation|SuspendResume|Terminate|Synchronize|VMOperation|VMRead|VMWrite
	};
	enum class SystemProcessPriority
	{
		None = -1,

		Idle,
		BelowNormal,
		Normal,
		AboveNormal,
		High,
		Realtime,
	};
	enum class CreateSystemProcessFlag
	{
		None = 0,

		WaitInputIdle = 1 << 0,
		WaitTermination = 1 << 1,
		CreateSuspended = 1 << 2,
		ToggleElevation = 1 << 3,
		RedirectStdIO = 1 << 4,

		Async = 1 << 16
	};

	Kx_DeclareFlagSet(MemoryProtection);
	Kx_DeclareFlagSet(SHWindowCommand);

	Kx_DeclareFlagSet(SystemProcessAccess);
	Kx_DeclareFlagSet(CreateSystemProcessFlag);
}
