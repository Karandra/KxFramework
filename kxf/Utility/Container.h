#pragma once
#include "kxf/Common.hpp"
#include <vector>
#include <utility>
#include <algorithm>
#include <type_traits>

namespace kxf::Utility
{
	template<class TOut, class TSource, class TFunc>
	void ConvertVector(std::vector<TOut>& destination, const std::vector<TSource>& source, TFunc&& func)
	{
		destination.reserve(destination.size() + source.size());
		for (const TSource& item: source)
		{
			destination.emplace_back(std::invoke(func, item));
		}
	}

	template<class TOut, class TSource, class TFunc>
	std::vector<TOut> ConvertVector(const std::vector<TSource>& source, TFunc&& func)
	{
		std::vector<TOut> destination;
		ConvertVector(destination, source, std::forward<TFunc>(func));

		return destination;
	}
}

namespace kxf::Utility
{
	template<class TContainer, class TFunc>
	auto FindIf(TContainer&& container, TFunc&& func) noexcept
	{
		return std::find_if(std::begin(std::forward<TContainer>(container)), std::end(std::forward<TContainer>(container)), std::forward<TFunc>(func));
	}

	template<class TContainer, class TFunc>
	bool Contains(const TContainer& container, TFunc&& func) noexcept
	{
		return FindIf(container, std::forward<TFunc>(func)) != std::end(container);
	}

	template<class TContainer, class TFunc>
	auto RemoveAllIf(TContainer& container, TFunc&& func)
	{
		return container.erase(std::remove_if(std::begin(container), std::end(container), std::forward<TFunc>(func)), std::end(container));
	}

	template<class TContainer, class TFunc>
	auto RemoveSingleIf(TContainer& container, TFunc&& func)
	{
		auto it = FindIf(container, std::forward<TFunc>(func));
		if (it != std::end(container))
		{
			return container.erase(it);
		}
		return it;
	}
}
