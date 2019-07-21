#pragma once
#include <type_traits>

namespace Kx::Utility
{
	template<class TLeft, class TRight> void ExchangeAndReset(TLeft& left, TLeft& right, TRight nullValue)
	{
		static_assert(std::is_trivially_move_assignable_v<TLeft> && std::is_trivially_move_assignable_v<TRight>,
					  "can only use ExchangeAndReset for trivially move assignable types");

		left = right;
		right = std::move(nullValue);
	}
	template<class TLeft, class TRight> TLeft ExchangeResetAndReturn(TLeft& right, TRight nullValue)
	{
		static_assert(std::is_default_constructible_v<TLeft>, "left type must be default constructible");

		TLeft left{};
		ExchangeAndReset(left, right, std::move(nullValue));
		return left;
	}

	template<class TFunc, class... Args>
	void ForEachParameterInPack(TFunc&& func, Args&&... arg)
	{
		std::initializer_list<int>{(func(std::forward<Args>(arg)), 0)...};
	}
}
