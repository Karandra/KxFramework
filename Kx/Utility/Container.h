#pragma once
#include "Kx/Common.hpp"
#include <vector>
#include <utility>
#include <type_traits>

namespace KxFramework::Utility
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
