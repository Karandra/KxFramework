#pragma once
#include "Common.h"
#include "IEnumerator.h"
#include "OptionalRef.h"

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
				using Result = IEnumerator::Result;
				switch (m_Enumerator->MoveNext())
				{
					case Result::Terminate:
					{
						// Stop immediately
						m_Enumerator = nullptr;

						break;
					}
					case Result::SkipCurrent:
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
			using TValueContainer = typename std::conditional_t<std::is_reference_v<TValue>, kxf::optional_ref<std::remove_reference_t<TValue>>, std::optional<TValue>>;

			using iterator = Private::EnumIterator<Enumerator>;

		private:
			static inline constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			std::function<TValueContainer(IEnumerator&)> m_MoveNext;
			TValueContainer m_Value;

			size_t m_Index = 0;
			size_t m_TotalCount = npos;
			bool m_IsReset = true;
			bool m_SkipCurrent = false;
			bool m_Terminate = false;

		private:
			Result DoMoveNext()
			{
				if (m_Index == npos || m_Index >= m_TotalCount)
				{
					return Result::Terminate;
				}

				Result result = InvokeProducer();
				if (result != Result::Terminate)
				{
					m_Index++;
				}
				else
				{
					m_Index = npos;
				}
				return result;
			}
			Result InvokeProducer()
			{
				m_Value = std::invoke(m_MoveNext, static_cast<IEnumerator&>(*this));

				if (m_Terminate)
				{
					m_Terminate = false;
					return Result::Terminate;
				}
				else if (m_SkipCurrent)
				{
					m_SkipCurrent = false;
					return Result::SkipCurrent;
				}
				else if (m_Value.has_value())
				{
					return Result::Continue;
				}
				return Result::Terminate;
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

			Enumerator(const Enumerator&) = default;
			Enumerator(Enumerator&&) noexcept = default;

		public:
			// IEnumerator
			bool IsNull() const noexcept override
			{
				return !static_cast<bool>(m_MoveNext);
			}

			Result MoveNext() override
			{
				Result result = DoMoveNext();
				m_IsReset = result == Result::Terminate;

				return result;
			}
			void SkipCurrent() noexcept override
			{
				m_SkipCurrent = true;
				m_Terminate = false;
			}
			void Terminate() noexcept override
			{
				m_SkipCurrent = false;
				m_Terminate = true;
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

				m_SkipCurrent = false;
				m_Terminate = false;
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

			const TValue& operator*() const& noexcept
			{
				return *m_Value;
			}
			TValue& operator*() & noexcept
			{
				return *m_Value;
			}
			TValue operator*() && noexcept
			{
				return *std::move(m_Value);
			}

			const TValue* operator->() const& noexcept
			{
				return &*m_Value;
			}
			TValue* operator->() & noexcept
			{
				return &*m_Value;
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
