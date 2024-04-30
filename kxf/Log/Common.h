#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"

namespace kxf
{
	enum class LogLevel
	{
		FlowControl = -1,
		Disabled = 0,

		Critical,
		Error,
		Warning,
		Information,
		Debug,
		Trace,

		Unknown
	};
}

namespace kxf::Log
{
	KX_API bool IsEnabled() noexcept;
	KX_API bool IsLevelEnabled(LogLevel level) noexcept;
	KX_API void Enable(LogLevel level) noexcept;
}
