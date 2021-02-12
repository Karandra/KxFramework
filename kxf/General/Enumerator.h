#pragma once
#include "Common.h"
#include "IEnumerator.h"
#include "OptionalRef.h"
#include "UniqueFunction.h"

namespace kxf::Private
{
	template<class TEnumerator>
	class EnumIterator
    {
		public:
			using value_type = typename TEnumerator::TValue;

		private:
			TEnumerator* m_Enumerator = nullptr;

		private:
			void DoMove()
			{
				switch (m_Enumerator->MoveNext())
				{
					case EnumeratorInstruction::Terminate:
					{
						// Stop immediately
						m_Enumerator = nullptr;

						break;
					}
					case EnumeratorInstruction::SkipCurrent:
					{
						// Advance to next step
						DoMove();

						break;
					}
				};
			}

        public:
			EnumIterator() noexcept = default;
            EnumIterator(TEnumerator& enumerator) noexcept
				:m_Enumerator(&enumerator)
			{
			}

		public:
			EnumIterator& operator++() noexcept
			{
				DoMove();
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
				return const_cast<const TEnumerator&>(*m_Enumerator).GetValue();
			}
			value_type& operator*() & noexcept
			{
				return const_cast<TEnumerator&>(*m_Enumerator).GetValue();
			}
			value_type operator*() && noexcept
			{
				return std::move(*m_Enumerator).GetValue();
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
			using TValueContainer = typename std::conditional_t<std::is_reference_v<TValue>, kxf::optional_ref<std::remove_reference_t<TValue>>, std::optional<TValue>>;

			using iterator = Private::EnumIterator<Enumerator>;

		private:
			static inline constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			kxf::unique_function<TValueContainer(IEnumerator&)> m_MoveNext;
			TValueContainer m_CurrentValue;
			EnumeratorInstruction m_CurrentInstruction = EnumeratorInstruction::Terminate;
			EnumeratorInstruction m_NextInstruction = EnumeratorInstruction::Continue;

			size_t m_Index = 0;
			size_t m_TotalCount = npos;

		private:
			EnumeratorInstruction DoMoveNext()
			{
				if (m_Index == npos || m_Index >= m_TotalCount)
				{
					return EnumeratorInstruction::Terminate;
				}

				EnumeratorInstruction result = InvokeProducer();
				if (result != EnumeratorInstruction::Terminate)
				{
					m_Index++;
				}
				else
				{
					m_Index = npos;
				}
				return result;
			}
			EnumeratorInstruction InvokeProducer()
			{
				m_CurrentValue = std::invoke(m_MoveNext, static_cast<IEnumerator&>(*this));

				if (m_NextInstruction == EnumeratorInstruction::Terminate)
				{
					return EnumeratorInstruction::Terminate;
				}
				else if (m_NextInstruction == EnumeratorInstruction::SkipCurrent)
				{
					return EnumeratorInstruction::SkipCurrent;
				}
				else if (m_CurrentValue.has_value())
				{
					return EnumeratorInstruction::Continue;
				}
				return EnumeratorInstruction::Terminate;
			}

			template<class T>
			static decltype(auto) DoGetValue(T&& items) noexcept
			{
				return *items;
			}

		public:
			Enumerator() noexcept = default;

			// TValueContainer func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, TValueContainer>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos)), m_MoveNext(std::forward<TFunc>(func))
			{
			}

			// TValueContainer func(void);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc>, TValueContainer>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos))
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> TValueContainer
				{
					return std::invoke(func);
				};
			}

			// TValue func(IEnumerator&);
			template<class TFunc, std::enable_if_t<std::is_same_v<std::invoke_result_t<TFunc, IEnumerator&>, TValue>, int> = 0>
			Enumerator(TFunc&& func, std::optional<size_t> count = {}) noexcept
				:m_TotalCount(count.value_or(npos))
			{
				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> TValueContainer
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

				m_MoveNext = [func = std::forward<TFunc>(func)](IEnumerator& enumerator) mutable -> TValueContainer
				{
					return std::invoke(func);
				};
			}

			Enumerator(const Enumerator&) = delete;
			Enumerator(Enumerator&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IEnumerator
			bool IsNull() const noexcept override
			{
				return m_MoveNext.is_null();
			}

			EnumeratorInstruction MoveNext() override
			{
				m_CurrentInstruction = DoMoveNext();
				m_NextInstruction = EnumeratorInstruction::Continue;

				return m_CurrentInstruction;
			}
			EnumeratorInstruction GetCurrentInstruction() const noexcept override
			{
				return m_CurrentInstruction;
			}
			void SkipCurrent() noexcept override
			{
				m_NextInstruction = EnumeratorInstruction::SkipCurrent;
			}
			void Terminate() noexcept override
			{
				m_NextInstruction = EnumeratorInstruction::Terminate;
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
				return m_CurrentInstruction == EnumeratorInstruction::Terminate || m_Index == npos;
			}
			void Reset() noexcept override
			{
				m_Index = 0;
				m_CurrentInstruction = EnumeratorInstruction::Terminate;
				m_NextInstruction = EnumeratorInstruction::Continue;
			}

			// Enumerator
			size_t CalcTotalCount()
			{
				size_t count = 0;
				while (MoveNext() == EnumeratorInstruction::Continue)
				{
					count++;
				}
				return count;
			}

			decltype(auto) GetValue() const& noexcept
			{
				return DoGetValue(m_CurrentValue);
			}
			decltype(auto) GetValue() & noexcept
			{
				return DoGetValue(m_CurrentValue);
			}
			decltype(auto) GetValue() && noexcept
			{
				return DoGetValue(std::move(m_CurrentValue));
			}

		public:
			iterator begin() noexcept
			{
				if (m_MoveNext)
				{
					iterator it = *this;
					++it;
					return it;
				}
				return {};
			}
			iterator end() noexcept
			{
				return {};
			}

			decltype(auto) operator*() const& noexcept
			{
				return DoGetValue(m_CurrentValue);
			}
			decltype(auto) operator*() & noexcept
			{
				return DoGetValue(m_CurrentValue);
			}
			decltype(auto) operator*() && noexcept
			{
				return DoGetValue(std::move(m_CurrentValue));
			}

			decltype(auto) operator->() const& noexcept
			{
				return &DoGetValue(m_CurrentValue);
			}
			decltype(auto) operator->() & noexcept
			{
				return &DoGetValue(m_CurrentValue);
			}

		public:
			bool operator==(const Enumerator& other) const noexcept
			{
				return (this == &other && m_Index == other.m_Index) || IsReset() && other.IsReset();
			}
			bool operator!=(const Enumerator& other) const noexcept
			{
				return !(*this == other);
			}

			Enumerator& operator=(const Enumerator&) = delete;
			Enumerator& operator=(Enumerator&& other) noexcept
			{
				m_MoveNext = std::move(other.m_MoveNext);
				m_CurrentValue = std::move(other.m_CurrentValue);

				m_CurrentInstruction = Utility::ExchangeResetAndReturn(other.m_CurrentInstruction, EnumeratorInstruction::Terminate);
				m_NextInstruction = Utility::ExchangeResetAndReturn(other.m_NextInstruction, EnumeratorInstruction::Continue);

				m_Index = Utility::ExchangeResetAndReturn(other.m_Index, 0);
				m_TotalCount = Utility::ExchangeResetAndReturn(other.m_TotalCount, npos);

				return *this;
			}
	};
}
