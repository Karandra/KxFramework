#pragma once
#include "Common.h"
#include <utility>
#include <memory>

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

namespace kxf::Utility
{
	struct Unfancy
	{
		template<class T>
		T& operator()(std::unique_ptr<T>& ptr) const noexcept
		{
			return *ptr;
		}

		template<class T>
		const T& operator()(const std::unique_ptr<T>& ptr) const noexcept
		{
			return *ptr;
		}

		template<class T>
		T& operator()(std::shared_ptr<T>& ptr) const noexcept
		{
			return *ptr;
		}

		template<class T>
		const T& operator()(const std::shared_ptr<T>& ptr) const noexcept
		{
			return *ptr;
		}
	};
}
