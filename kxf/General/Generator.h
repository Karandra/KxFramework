#pragma once
#include "Common.h"

namespace kxf
{
	template<class TValue>
	class GeneratorOf;
}

namespace kxf::Private
{
	template<class TGenerator>
	class GeneratorOfIterator
    {
		template<class TValue>
		friend class GeneratorOf;

		public:
			using value_type = typename TGenerator::TValue;

		private:
			TGenerator* m_Generator = nullptr;

        private:
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
	class GeneratorOf final
	{
		public:
			using TValue = TValue_;
			using iterator = Private::GeneratorOfIterator<GeneratorOf>;

		private:
			static inline constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			std::function<std::optional<TValue>(size_t)> m_Generator;

			std::optional<TValue> m_Value;
			size_t m_Index = 0;
			size_t m_TotalCount = npos;
			bool m_IsReset = true;

		private:
			bool DoMoveNext(bool dryRun = false) noexcept
			{
				if (m_Index == npos || m_Index >= m_TotalCount)
				{
					return false;
				}

				m_Value = std::invoke(m_Generator, m_Index);
				if (m_Value || (dryRun && m_TotalCount != npos))
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

		public:
			GeneratorOf() noexcept = default;

			template<class TFunc, std::enable_if_t<std::is_invocable_r_v<std::optional<TValue>, TFunc, size_t>, int> = 0>
			GeneratorOf(TFunc&& generator, size_t count = npos) noexcept
				:m_Generator(std::forward<TFunc>(generator)), m_TotalCount(count)
			{
			}

			GeneratorOf(const GeneratorOf&) = default;
			GeneratorOf(GeneratorOf&&) noexcept = default;

		public:
			bool IsNull() const noexcept
			{
				return m_IsReset || m_Index == npos;
			}
			bool IsTotalCountKnown() const noexcept
			{
				return m_TotalCount != npos;
			}
			size_t GetMoveCount() noexcept
			{
				return m_Index;
			}
			size_t DryRun() noexcept
			{
				size_t count = 0;
				if (IsTotalCountKnown())
				{
					count = m_TotalCount;
				}
				else
				{
					while (DoMoveNext(true))
					{
						count++;
					};
				}

				m_IsReset = true;
				return count;
			}

			bool MoveNext() noexcept
			{
				bool result = DoMoveNext();
				m_IsReset = !result;

				return result;
			}
			void Reset() noexcept
			{
				m_Value.reset();
				m_Index = 0;
				m_IsReset = true;
			}

			const TValue& GetValue() const& noexcept
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
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const GeneratorOf& other) const noexcept
			{
				return (this == &other && m_Index == other.m_Index) || m_IsReset && other.m_IsReset;
			}
			bool operator!=(const GeneratorOf& other) const noexcept
			{
				return !(*this == other);
			}

			GeneratorOf& operator=(const GeneratorOf&) = default;
			GeneratorOf& operator=(GeneratorOf&&) noexcept = default;
	};
}
