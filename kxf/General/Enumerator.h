#pragma once
#include "Common.h"
#include "IEnumerator.h"

namespace kxf
{
	class AbstractEnumerator: public RTTI::Implementation<AbstractEnumerator, IEnumerator>
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

		protected:
			AbstractEnumerator(std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos))
			{
			}
			AbstractEnumerator(const AbstractEnumerator&) = default;
			AbstractEnumerator(AbstractEnumerator&&) noexcept = default;
			virtual ~AbstractEnumerator() = default;

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

			bool operator==(const AbstractEnumerator& other) const noexcept
			{
				return (this == &other && m_Index == other.m_Index) || m_IsReset && other.m_IsReset;
			}
			bool operator!=(const AbstractEnumerator& other) const noexcept
			{
				return !(*this == other);
			}

			AbstractEnumerator& operator=(const AbstractEnumerator&) = default;
			AbstractEnumerator& operator=(AbstractEnumerator&&) noexcept = default;
	};
}

namespace kxf::Private
{
	template<class TEnumerator>
	class EnumIterator
    {
		public:
			using value_type = typename TEnumerator::TValue;

		private:
			TEnumerator* m_Enumerator = nullptr;

        public:
			EnumIterator() noexcept = default;
            EnumIterator(TEnumerator& enumerator) noexcept
				:m_Enumerator(&enumerator)
			{
			}

		public:
			EnumIterator& operator++() noexcept
			{
				if (!m_Enumerator->MoveNext())
				{
					m_Enumerator = nullptr;
				}
				return *this;
			}
			EnumIterator operator++(int) const noexcept
			{
				EnumIterator clone = *this;
				++clone;

				return clone;
			}

			const value_type& operator*() const& noexcept
			{
				return m_Enumerator->GetValue();
			}
			value_type& operator*() & noexcept
			{
				return m_Enumerator->GetValue();
			}
			value_type operator*() && noexcept
			{
				return m_Enumerator->GetValue();
			}

			const TEnumerator* operator->() const noexcept
			{
				return m_Enumerator;
			}
			TEnumerator* operator->() noexcept
			{
				return m_Enumerator;
			}

		public:
			bool operator==(const EnumIterator& other) const noexcept
			{
				return this == &other || m_Enumerator == other.m_Enumerator;
			}
			bool operator!=(const EnumIterator& other) const noexcept
			{
				return !(*this == other);
			}
    };
}

namespace kxf
{
	template<class TValue_>
	class Enumerator final: public AbstractEnumerator
	{
		public:
			using TValue = TValue_;
			using iterator = Private::EnumIterator<Enumerator>;

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
			Enumerator() noexcept = default;

			// std::optional<TValue> func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, std::optional<TValue>>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:AbstractEnumerator(std::move(count)), m_MoveNext(std::forward<TFunc>(func))
			{
			}

			// std::optional<TValue> func(void);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc>, std::optional<TValue>>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:AbstractEnumerator(std::move(count))
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func);
				};
			}

			// TValue func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, TValue>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:AbstractEnumerator(std::move(count))
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func, enumerator);
				};
			}

			// TValue func(void);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc>, TValue>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:AbstractEnumerator(std::move(count))
			{
				wxASSERT_MSG(count.has_value() && *count != npos, "Producer function with no way to signal termination must only be used with known total limit");

				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func);
				};
			}

			Enumerator(const Enumerator&) = default;
			Enumerator(Enumerator&&) noexcept = default;

		public:
			// IEnumerator
			void Terminate() noexcept override
			{
				m_TerminationRequested = true;
			}
			void Reset() noexcept override
			{
				AbstractEnumerator::Reset();
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
