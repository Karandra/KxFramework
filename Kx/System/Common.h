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
}
