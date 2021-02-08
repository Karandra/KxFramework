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
	Enumerator<TValue> MakeEnumerator(TFunc&& func, std::optional<size_t> count = {})
	{
		return {std::forward<TFunc>(func), std::move(count)};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc, IEnumerator&>,
		std::enable_if_t<is_optional_v<TValue> && std::is_invocable_v<TFunc, IEnumerator&>, int> = 0
	>
	Enumerator<typename TValue::value_type> MakeEnumerator(TFunc&& func, std::optional<size_t> count = {})
	{
		return {std::forward<TFunc>(func), std::move(count)};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc>,
		std::enable_if_t<!is_optional_v<TValue>, int> = 0
	>
	Enumerator<TValue> MakeEnumerator(TFunc&& func, std::optional<size_t> count = {})
	{
		return {std::forward<TFunc>(func), std::move(count)};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc>,
		std::enable_if_t<is_optional_v<TValue>, int> = 0
	>
	Enumerator<typename TValue::value_type> MakeEnumerator(TFunc&& func, std::optional<size_t> count = {})
	{
		return {std::forward<TFunc>(func), std::move(count)};
	}
}

namespace kxf::Utility
{
	template<class TConverterFunc, class TOwner, class TEnumFunc, class... Args, std::enable_if_t<std::is_member_function_pointer_v<TEnumFunc>, int> = 0>
	decltype(auto) MakeForwardingEnumerator(TConverterFunc&& converter, TOwner owner, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = typename std::invoke_result_t<TEnumFunc, TOwner*, Args...>;
		using TValue = typename TEnumerator::TValue;

		class Context final
		{
			private:
				TOwner m_Owner;
				TEnumerator m_Range;

			public:
				Context(TOwner owner, TEnumFunc enumFunc, Args&&... arg)
					:m_Owner(std::move(owner)), m_Range(std::invoke(enumFunc, &m_Owner, std::forward<Args>(arg)...))
				{
				}

			public:
				TOwner& GetOwner()
				{
					return m_Owner;
				}
				TEnumerator& GetEnumerator()
				{
					return m_Range;
				}

				IEnumerator::Result MoveNext()
				{
					return m_Range.MoveNext();
				}
		};

		// Unfortunately we have to use 'std::shared_ptr<T>' because lambda that contains move-only type cannot be stored inside 'std::function'
		// as it requires its target to be copyable. Though it's a shame that we have to use a heap-allocated object at all just to forward
		// an enumerator.
		auto context = std::make_shared<Context>(std::move(owner), enumFunc, std::forward<Args>(arg)...);
		auto totalCount = context->GetEnumerator().GetTotalCount();

		return Utility::MakeEnumerator([converter = std::forward<TConverterFunc>(converter), context = std::move(context)](IEnumerator& enumerator) mutable -> std::optional<TValue>
		{
			using Result = IEnumerator::Result;
			switch (context->MoveNext())
			{
				case Result::Continue:
				{
					if constexpr(std::is_invocable_v<TConverterFunc, TEnumerator&, TOwner&>)
					{
						return std::invoke(converter, context->GetEnumerator(), context->GetOwner());
					}
					else
					{
						return std::invoke(converter, context->GetEnumerator());
					}
				}
				case Result::SkipCurrent:
				{
					enumerator.SkipCurrent();
					break;
				}
				case Result::Terminate:
				{
					enumerator.Terminate();
					break;
				}
			};
			return {};
		}, std::move(totalCount));
	}

	template<class TConverterFunc, class TEnumFunc, class... Args>
	decltype(auto) MakeForwardingEnumerator(TConverterFunc&& converter, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = typename std::invoke_result_t<TEnumFunc, Args...>;
		using TValue = typename TEnumerator::TValue;

		static_assert(!std::is_member_function_pointer_v<TEnumFunc>, "free/static function pointer required");
		static_assert(std::is_invocable_r_v<std::optional<TValue>, TConverterFunc, TEnumerator&>, "invalid converter function signature");

		class Context final
		{
			private:
				TEnumerator m_Range;

			public:
				Context(TEnumFunc enumFunc, Args&&... arg)
					:m_Range(std::invoke(enumFunc, std::forward<Args>(arg)...))
				{
				}

			public:
				TEnumerator& GetEnumerator()
				{
					return m_Range;
				}

				IEnumerator::Result MoveNext()
				{
					return m_Range.MoveNext();
				}
		};

		// Look overload above for details about using a shared pointer
		auto context = std::make_shared<Context>(enumFunc, std::forward<Args>(arg)...);
		auto totalCount = context->GetEnumerator().GetTotalCount();

		return Utility::MakeEnumerator([converter = std::forward<TConverterFunc>(converter), context = std::move(context)](IEnumerator& enumerator) mutable -> std::optional<TValue>
		{
			using Result = IEnumerator::Result;
			switch (context->MoveNext())
			{
				case Result::Continue:
				{
					return std::invoke(converter, context->GetEnumerator());
				}
				case Result::SkipCurrent:
				{
					enumerator.SkipCurrent();
					break;
				}
				case Result::Terminate:
				{
					enumerator.Terminate();
					break;
				}
			};
			return {};
		}, std::move(totalCount));
	}
}
