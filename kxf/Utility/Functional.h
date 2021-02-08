#pragma once
#include "Common.h"
#include <utility>

namespace kxf::Utility
{
	struct identity
	{
		static_assert(__cplusplus < 201703L, "use 'std::identity' instead");

		template<class T>
		constexpr auto operator()(T&& v) const noexcept -> decltype(std::forward<T>(v))
		{
			return std::forward<T>(v);
		}
	};
}
