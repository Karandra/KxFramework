#pragma once
#include "Common.h"
#include <optional>
#include <functional>

namespace kxf
{
	template<class T>
	using optional_ref = std::optional<std::reference_wrapper<T>>;
}
