#pragma once
#include "Common.h"
#include "TypeTraits.h"
#include "Functional.h"
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
	template<bool forwardTotalCount = false, class TConvFunc, class TOwner, class TEnumFunc, class... Args, std::enable_if_t<std::is_member_function_pointer_v<TEnumFunc>, int> = 0>
	decltype(auto) MakeForwardingEnumerator(TConvFunc&& conv, TOwner&& owner, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = typename std::invoke_result_t<TEnumFunc, std::remove_reference_t<TOwner>*, Args...>;
		using TValue = typename TEnumerator::TValue;
		using TValueContainer = typename TEnumerator::TValueContainer;

		class Context final
		{
			private:
				TOwner&& m_Owner;
				TEnumerator m_Range;

			public:
				Context(TOwner&& owner, TEnumFunc enumFunc, Args&&... arg)
					:m_Owner(std::forward<TOwner>(owner)), m_Range(std::invoke(enumFunc, &m_Owner, std::forward<Args>(arg)...))
				{
				}

			public:
				TOwner& GetOwner() noexcept
				{
					return m_Owner;
				}
				TEnumerator& GetEnumerator() noexcept
				{
					return m_Range;
				}

				std::optional<size_t> GetTotalCount() const noexcept
				{
					if (forwardTotalCount)
					{
						return m_Range.GetTotalCount();
					}
					return {};
				}
				IEnumerator::Result MoveNext()
				{
					return m_Range.MoveNext();
				}
		};

		// Unfortunately we have to use 'std::shared_ptr<T>' because lambda that contains move-only type cannot be stored inside 'std::function'
		// as it requires its target to be copyable. Though it's a shame that we have to use a heap-allocated object at all just to forward
		// an enumerator.
		auto context = std::make_shared<Context>(std::forward<TOwner>(owner), enumFunc, std::forward<Args>(arg)...);
		auto totalCount = context->GetTotalCount();

		return TEnumerator([conv = std::forward<TConvFunc>(conv), context = std::move(context)](IEnumerator& enumerator) mutable -> TValueContainer
		{
			using Result = IEnumerator::Result;
			switch (context->MoveNext())
			{
				case Result::Continue:
				{
					if constexpr(std::is_invocable_v<TConvFunc, TEnumerator&, TOwner&>)
					{
						return std::invoke(conv, context->GetEnumerator(), context->GetOwner());
					}
					else
					{
						return std::invoke(conv, context->GetEnumerator());
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

	template<bool forwardTotalCount = false, class TConvFunc, class TEnumFunc, class... Args>
	decltype(auto) MakeForwardingEnumerator(TConvFunc&& conv, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = typename std::invoke_result_t<TEnumFunc, Args...>;
		using TValue = typename TEnumerator::TValue;
		using TValueContainer = typename TEnumerator::TValueContainer;

		static_assert(!std::is_member_function_pointer_v<TEnumFunc>, "free/static function pointer required");
		static_assert(std::is_invocable_r_v<std::optional<TValue>, TConvFunc, TEnumerator&>, "invalid converter function signature");

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
				TEnumerator& GetEnumerator() noexcept
				{
					return m_Range;
				}

				std::optional<size_t> GetTotalCount() const noexcept
				{
					if (forwardTotalCount)
					{
						return m_Range.GetTotalCount();
					}
					return {};
				}
				IEnumerator::Result MoveNext()
				{
					return m_Range.MoveNext();
				}
		};

		// Look overload above for details about using a shared pointer
		auto context = std::make_shared<Context>(enumFunc, std::forward<Args>(arg)...);
		auto totalCount = context->GetTotalCount();

		return TEnumerator([conv = std::forward<TConvFunc>(conv), context = std::move(context)](IEnumerator& enumerator) mutable -> TValueContainer
		{
			using Result = IEnumerator::Result;
			switch (context->MoveNext())
			{
				case Result::Continue:
				{
					return std::invoke(conv, context->GetEnumerator());
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

namespace kxf::Utility
{
	template<class TValue, class TConvFunc = identity, class TContainer>
	Enumerator<TValue> EnumerateIndexableContainer(TContainer&& container, TConvFunc&& conv = {})
	{
		size_t count = std::size(container);
		return MakeEnumerator([container = std::forward_as_tuple(container), conv = std::forward<TConvFunc>(conv), index = 0_zu]() mutable -> TValue
		{
			return std::forward<TValue>(std::invoke(conv, std::get<0>(container)[index++]));
		}, count);
	}

	template<class TValue, class TConvFunc = identity, class TExtractFunc = identity, class TContainer>
	Enumerator<TValue> EnumerateIterableContainer(TContainer&& container, TConvFunc&& conv = {}, TExtractFunc&& extract = {})
	{
		using Tx = std::remove_reference_t<TContainer>;
		using TIterator = std::conditional_t<std::is_const_v<Tx>, typename Tx::const_iterator, typename Tx::iterator>;

		const size_t count = container.size();
		return MakeEnumerator([container = std::forward_as_tuple(container),
							  it = std::optional<TIterator>(),
							  conv = std::forward<TConvFunc>(conv),
							  extract = std::forward<TExtractFunc>(extract)
		]() mutable -> TValue
		{
			// Initialize the iterator
			if (!it)
			{
				it = std::get<0>(container).begin();
			}

			// Extract and convert the value and advance the iterator
			decltype(auto) result = std::invoke(conv, std::invoke(extract, *(*it)));
			++(*it);

			// Return it
			return std::forward<TValue>(result);
		}, count);
	}

	template<class TValue, class TConvFunc = identity, class TContainer>
	Enumerator<TValue> EnumerateStandardMap(TContainer&& container, TConvFunc&& conv = {})
	{
		return EnumerateIterableContainer<TValue>(std::forward<TContainer>(container), std::forward<TConvFunc>(conv), identity_tuple<1>());
	}
}
