#pragma once
#include "Common.h"
#include "TypeTraits.h"
#include "kxf/General/Enumerator.h"

namespace kxf::Utility
{
	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc, IEnumerator&>,
		std::enable_if_t<!is_optional_v<TValue> && std::is_invocable_v<TFunc, IEnumerator&>, int> = 0
	>
	Enumerator<TValue> MakeEnumerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc, IEnumerator&>,
		std::enable_if_t<is_optional_v<TValue> && std::is_invocable_v<TFunc, IEnumerator&>, int> = 0
	>
	Enumerator<typename TValue::value_type> MakeEnumerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc>,
		std::enable_if_t<!is_optional_v<TValue>, int> = 0
	>
	Enumerator<TValue> MakeEnumerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc>,
		std::enable_if_t<is_optional_v<TValue>, int> = 0
	>
	Enumerator<typename TValue::value_type> MakeEnumerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}
}
