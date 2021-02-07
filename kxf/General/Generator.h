#pragma once
#include "Common.h"
#include "IEnumerator.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf
{
	class AbstractGenerator: public RTTI::Implementation<AbstractGenerator, IEnumerator>
	{
		protected:
			static inline constexpr size_t npos = std::numeric_limits<size_t>::max();

		protected:
			size_t m_Index = 0;
			size_t m_TotalCount = npos;
			bool m_IsReset = true;

		private:
			bool DoMoveNext() noexcept;

		protected:
			virtual bool InvokeGenerator() = 0;

		public:
			AbstractGenerator(size_t count = npos) noexcept
				:m_TotalCount(count)
			{
			}
			AbstractGenerator(const AbstractGenerator&) = default;
			AbstractGenerator(AbstractGenerator&&) noexcept = default;
			virtual ~AbstractGenerator() = default;

		public:
			// IEnumerator
			bool MoveNext() noexcept override
			{
				bool result = DoMoveNext();
				m_IsReset = !result;

				return result;
			}
			size_t GetCurrentStep() const noexcept override
			{
				return m_Index;
			}

			bool IsReset() const noexcept override
			{
				return m_IsReset || m_Index == npos;
			}
			void Reset() noexcept override
			{
				m_Index = 0;
				m_IsReset = true;
			}

			// GeneratorCommon
			std::optional<size_t> GetTotalCount() noexcept
			{
				if (m_TotalCount != npos)
				{
					return m_TotalCount;
				}
				return {};
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsReset();
			}
			bool operator!() const noexcept
			{
				return IsReset();
			}

			bool operator==(const AbstractGenerator& other) const noexcept
			{
				return (this == &other && m_Index == other.m_Index) || m_IsReset && other.m_IsReset;
			}
			bool operator!=(const AbstractGenerator& other) const noexcept
			{
				return !(*this == other);
			}

			AbstractGenerator& operator=(const AbstractGenerator&) = default;
			AbstractGenerator& operator=(AbstractGenerator&&) noexcept = default;
	};
}

namespace kxf::Private
{
	template<class TGenerator>
	class GeneratorOfIterator
    {
		public:
			using value_type = typename TGenerator::TValue;

		private:
			TGenerator* m_Generator = nullptr;

        public:
			GeneratorOfIterator() noexcept = default;
            GeneratorOfIterator(TGenerator& generator) noexcept
				:m_Generator(&generator)
			{
			}

		public:
			GeneratorOfIterator& operator++() noexcept
			{
				if (!m_Generator->MoveNext())
				{
					m_Generator = nullptr;
				}
				return *this;
			}
			GeneratorOfIterator operator++(int) const noexcept
			{
				GeneratorOfIterator clone = *this;
				++clone;

				return clone;
			}

			const value_type& operator*() const& noexcept
			{
				return m_Generator->GetValue();
			}
			value_type& operator*() & noexcept
			{
				return m_Generator->GetValue();
			}
			value_type operator*() && noexcept
			{
				return m_Generator->GetValue();
			}

			const TGenerator* operator->() const noexcept
			{
				return m_Generator;
			}
			TGenerator* operator->() noexcept
			{
				return m_Generator;
			}

		public:
			bool operator==(const GeneratorOfIterator& other) const noexcept
			{
				return this == &other || m_Generator == other.m_Generator;
			}
			bool operator!=(const GeneratorOfIterator& other) const noexcept
			{
				return !(*this == other);
			}
    };
}

namespace kxf
{
	template<class TValue_>
	class GeneratorOf final: public AbstractGenerator
	{
		public:
			using TValue = TValue_;
			using iterator = Private::GeneratorOfIterator<GeneratorOf>;

		private:
			std::function<std::optional<TValue>(IEnumerator&)> m_MoveNext;
			std::optional<TValue> m_Value;
			bool m_TerminationRequested = false;

		protected:
			bool InvokeGenerator() override
			{
				m_Value = std::invoke(m_MoveNext, static_cast<IEnumerator&>(*this));
				return !m_TerminationRequested && m_Value.has_value();
			}

		public:
			GeneratorOf() noexcept = default;

			// std::optional<TValue> func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, std::optional<TValue>>, int> = 0>
			GeneratorOf(TFunc&& func, size_t count = npos) noexcept
				:AbstractGenerator(count), m_MoveNext(std::forward<TFunc>(func))
			{
			}

			// std::optional<TValue> func(void);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc>, std::optional<TValue>>, int> = 0>
			GeneratorOf(TFunc&& func, size_t count = npos) noexcept
				:AbstractGenerator(count)
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func);
				};
			}

			// TValue func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, TValue>, int> = 0>
			GeneratorOf(TFunc&& func, size_t count = npos) noexcept
				:AbstractGenerator(count)
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func, enumerator);
				};
			}

			// TValue func(void);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc>, TValue>, int> = 0>
			GeneratorOf(TFunc&& func, size_t count = npos) noexcept
				:AbstractGenerator(count)
			{
				wxASSERT_MSG(count != npos, "Producer function with no way to signal termination must only be used with known total limit");

				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func);
				};
			}

			GeneratorOf(const GeneratorOf&) = default;
			GeneratorOf(GeneratorOf&&) noexcept = default;

		public:
			// IEnumerator
			void Terminate() noexcept override
			{
				m_TerminationRequested = true;
			}
			void Reset() noexcept override
			{
				AbstractGenerator::Reset();
				m_TerminationRequested = false;
			}

			// GeneratorOf<TValue>
			const TValue& GetValue() const& noexcept
			{
				return *m_Value;
			}
			TValue& GetValue() & noexcept
			{
				return *m_Value;
			}
			TValue GetValue() && noexcept
			{
				return *std::move(m_Value);
			}

		public:
			iterator begin() noexcept
			{
				iterator it = *this;
				++it;
				return it;
			}
			iterator end() noexcept
			{
				return {};
			}
	};
}

namespace kxf
{
	template
		<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc, IEnumerator&>,
		std::enable_if_t<!Utility::is_optional_v<TValue> && std::is_invocable_v<TFunc, IEnumerator&>, int> = 0
	>
	GeneratorOf<TValue> MakeGenerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc, IEnumerator&>,
		std::enable_if_t<Utility::is_optional_v<TValue> && std::is_invocable_v<TFunc, IEnumerator&>, int> = 0
	>
	GeneratorOf<typename TValue::value_type> MakeGenerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc>,
		std::enable_if_t<!Utility::is_optional_v<TValue>, int> = 0
	>
	GeneratorOf<TValue> MakeGenerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}

	template
	<
		class TFunc,
		class TValue = std::invoke_result_t<TFunc>,
		std::enable_if_t<Utility::is_optional_v<TValue>, int> = 0
	>
	GeneratorOf<typename TValue::value_type> MakeGenerator(TFunc&& func, size_t count = std::numeric_limits<size_t>::max())
	{
		return {std::forward<TFunc>(func), count};
	}
}
