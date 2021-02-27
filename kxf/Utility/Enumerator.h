#pragma once
#include "Common.h"
#include "TypeTraits.h"
#include "Functional.h"
#include "Literals.h"
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
	template<class TValue, class TConvFunc, class TOwner, class TEnumFunc, class... Args, std::enable_if_t<std::is_member_function_pointer_v<TEnumFunc>, int> = 0>
	decltype(auto) MakeConvertingEnumerator(TConvFunc&& conv, TOwner&& owner, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = Enumerator<TValue>;
		using TValueContainer = typename TEnumerator::TValueContainer;

		class Context final
		{
			private:
				using TSourceEnumerator = typename std::invoke_result_t<TEnumFunc, std::remove_reference_t<TOwner>*, Args...>;

			private:
				TOwner&& m_Owner;
				TSourceEnumerator m_Range;

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
				TSourceEnumerator& GetEnumerator() noexcept
				{
					return m_Range;
				}

				EnumeratorInstruction MoveNext()
				{
					return m_Range.MoveNext();
				}
		};

		return TEnumerator([conv = std::forward<TConvFunc>(conv),
						   context = Context(std::forward<TOwner>(owner), enumFunc, std::forward<Args>(arg)...)]
						   (IEnumerator& enumerator) mutable -> TValueContainer
		{
			switch (context.MoveNext())
			{
				case EnumeratorInstruction::Continue:
				{
					return std::invoke(conv, std::move(context.GetEnumerator()).GetValue(), enumerator);
				}
				case EnumeratorInstruction::SkipCurrent:
				{
					enumerator.SkipCurrent();
					break;
				}
				case EnumeratorInstruction::Terminate:
				{
					enumerator.Terminate();
					break;
				}
			};
			return {};
		});
	}

	template<class TValue, class TConvFunc, class TEnumFunc, class... Args>
	decltype(auto) MakeConvertingEnumerator(TConvFunc&& conv, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = Enumerator<TValue>;
		using TValueContainer = typename TEnumerator::TValueContainer;

		static_assert(!std::is_member_function_pointer_v<TEnumFunc>, "free/static function pointer required");
		static_assert(std::is_invocable_r_v<std::optional<TValue>, TConvFunc, TEnumerator&>, "invalid converter function signature");

		class Context final
		{
			private:
				using TSourceEnumerator = typename std::invoke_result_t<TEnumFunc, Args...>;

			private:
				TSourceEnumerator m_Range;

			public:
				Context(TEnumFunc enumFunc, Args&&... arg)
					:m_Range(std::invoke(enumFunc, std::forward<Args>(arg)...))
				{
				}

			public:
				TSourceEnumerator& GetEnumerator() noexcept
				{
					return m_Range;
				}

				EnumeratorInstruction MoveNext()
				{
					return m_Range.MoveNext();
				}
		};

		return TEnumerator([conv = std::forward<TConvFunc>(conv),
						   context = Context(enumFunc, std::forward<Args>(arg)...)]
						   (IEnumerator& enumerator) mutable -> TValueContainer
		{
			switch (context.MoveNext())
			{
				case EnumeratorInstruction::Continue:
				{
					return std::invoke(conv, std::move(context.GetEnumerator()).GetValue(), enumerator);
				}
				case EnumeratorInstruction::SkipCurrent:
				{
					enumerator.SkipCurrent();
					break;
				}
				case EnumeratorInstruction::Terminate:
				{
					enumerator.Terminate();
					break;
				}
			};
			return {};
		});
	}

	template<class TConvFunc, class TOwner, class TEnumFunc, class... Args, std::enable_if_t<std::is_member_function_pointer_v<TEnumFunc>, int> = 0>
	decltype(auto) MakeForwardingEnumerator(TConvFunc&& conv, TOwner&& owner, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = typename std::invoke_result_t<TEnumFunc, std::remove_reference_t<TOwner>*, Args...>;
		using TValue = typename TEnumerator::TValue;

		return MakeConvertingEnumerator<TValue>(std::forward<TConvFunc>(conv), std::forward<TOwner>(owner), enumFunc, std::forward<Args>(arg)...);
	}

	template<class TConvFunc, class TEnumFunc, class... Args>
	decltype(auto) MakeForwardingEnumerator(TConvFunc&& conv, TEnumFunc enumFunc, Args&&... arg)
	{
		using TEnumerator = typename std::invoke_result_t<TEnumFunc, Args...>;
		using TValue = typename TEnumerator::TValue;

		return MakeConvertingEnumerator<TValue>(std::forward<TConvFunc>(conv), enumFunc, std::forward<Args>(arg)...);
	}
}

namespace kxf::Utility::Private
{
	template<class TValue, class TConvFunc = identity, class TExtractFunc = identity, class TBeginFunc, class TContainer>
	Enumerator<TValue> EnumerateIterableContainer(TContainer&& container, TConvFunc&& conv, TExtractFunc&& extract, TBeginFunc&& beginFunc)
	{
		using TIterator = std::invoke_result_t<TBeginFunc, TContainer>;

		const size_t count = std::size(container);
		return MakeEnumerator([container = std::forward_as_tuple(container),
							  it = std::optional<TIterator>(),
							  conv = std::forward<TConvFunc>(conv),
							  extract = std::forward<TExtractFunc>(extract),
							  beginFunc = std::forward<TBeginFunc>(beginFunc)
		]() mutable -> TValue
		{
			// Initialize the iterator
			if (!it)
			{
				it = std::invoke(beginFunc, std::get<0>(container));
			}

			// Extract and convert the value and advance the iterator
			decltype(auto) result = std::invoke(conv, std::invoke(extract, *(*it)));
			++(*it);

			// Return it
			return std::forward<TValue>(result);
		}, count);
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
		return Private::EnumerateIterableContainer<TValue>(std::forward<TContainer>(container),
														   std::forward<TConvFunc>(conv),
														   std::forward<TExtractFunc>(extract),
														   [](auto&& container)
		{
			return std::begin(container);
		});
	}

	template<class TValue, class TConvFunc = identity, class TExtractFunc = identity, class TContainer>
	Enumerator<TValue> EnumerateIterableContainerReverse(TContainer&& container, TConvFunc&& conv = {}, TExtractFunc&& extract = {})
	{
		return Private::EnumerateIterableContainer<TValue>(std::forward<TContainer>(container),
														   std::forward<TConvFunc>(conv),
														   std::forward<TExtractFunc>(extract),
														   [](auto&& container)
		{
			return std::rbegin(container);
		});
	}

	template<class TValue, class TConvFunc = identity, class TContainer>
	Enumerator<TValue> EnumerateStandardMap(TContainer&& container, TConvFunc&& conv = {})
	{
		return EnumerateIterableContainer<TValue>(std::forward<TContainer>(container), std::forward<TConvFunc>(conv), identity_tuple<1>());
	}
}
