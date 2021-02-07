#pragma once
#include "Common.h"
#include "IEnumerator.h"

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
	class Enumerator final: public IEnumerator
	{
		public:
			using TValue = TValue_;
			using iterator = Private::EnumIterator<Enumerator>;

		private:
			static inline constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			std::function<std::optional<TValue>(IEnumerator&)> m_MoveNext;
			std::optional<TValue> m_Value;

			size_t m_Index = 0;
			size_t m_TotalCount = npos;
			bool m_IsReset = true;
			bool m_TerminationRequested = false;

		private:
			bool DoMoveNext()
			{
				if (m_Index == npos || m_Index >= m_TotalCount)
				{
					return false;
				}

				if (InvokeProducer())
				{
					m_Index++;
					return true;
				}
				else
				{
					m_Index = npos;
					return false;
				}
			}
			bool InvokeProducer()
			{
				m_Value = std::invoke(m_MoveNext, static_cast<IEnumerator&>(*this));
				return !m_TerminationRequested && m_Value.has_value();
			}

		public:
			Enumerator() noexcept = default;

			// std::optional<TValue> func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, std::optional<TValue>>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos)), m_MoveNext(std::forward<TFunc>(func))
			{
			}

			// std::optional<TValue> func(void);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc>, std::optional<TValue>>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos))
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func);
				};
			}

			// TValue func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, TValue>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos))
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> std::optional<TValue>
				{
					return std::invoke(func, enumerator);
				};
			}

			// TValue func(void);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc>, TValue>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos))
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
			bool MoveNext() override
			{
				bool result = DoMoveNext();
				m_IsReset = !result;

				return result;
			}
			void Terminate() noexcept override
			{
				m_TerminationRequested = true;
			}
			size_t GetCurrentStep() const noexcept override
			{
				return m_Index;
			}
			std::optional<size_t> GetTotalCount() const noexcept override
			{
				if (m_TotalCount != npos)
				{
					return m_TotalCount;
				}
				return {};
			}

			bool IsReset() const noexcept override
			{
				return m_IsReset || m_Index == npos;
			}
			void Reset() noexcept override
			{
				m_Index = 0;
				m_IsReset = true;
				m_TerminationRequested = false;
			}

			// Enumerator
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

		public:
			bool operator==(const Enumerator& other) const noexcept
			{
				return (this == &other && m_Index == other.m_Index) || m_IsReset && other.m_IsReset;
			}
			bool operator!=(const Enumerator& other) const noexcept
			{
				return !(*this == other);
			}

			Enumerator& operator=(const Enumerator&) = default;
			Enumerator& operator=(Enumerator&&) noexcept = default;
	};
}
