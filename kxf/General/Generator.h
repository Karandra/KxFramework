#pragma once
#include "Common.h"

namespace kxf::Private
{
	class GeneratorCommon
	{
		protected:
			static inline constexpr size_t npos = std::numeric_limits<size_t>::max();

		protected:
			size_t m_Index = 0;
			size_t m_TotalCount = npos;
			bool m_IsReset = true;

		private:
			bool DoMoveNext(bool dryRun = false) noexcept;
			size_t DoDryRun() noexcept;

		protected:
			virtual bool InvokeGenerator(size_t index) = 0;

		public:
			GeneratorCommon(size_t count = npos) noexcept
				:m_TotalCount(count)
			{
			}
			GeneratorCommon(const GeneratorCommon&) = default;
			GeneratorCommon(GeneratorCommon&&) noexcept = default;
			virtual ~GeneratorCommon() = default;

		public:
			bool IsReset() const noexcept
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
				return DoDryRun();
			}

			bool MoveNext() noexcept
			{
				bool result = DoMoveNext();
				m_IsReset = !result;

				return result;
			}
			void Reset() noexcept
			{
				m_Index = 0;
				m_IsReset = true;
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

			bool operator==(const GeneratorCommon& other) const noexcept
			{
				return (this == &other && m_Index == other.m_Index) || m_IsReset && other.m_IsReset;
			}
			bool operator!=(const GeneratorCommon& other) const noexcept
			{
				return !(*this == other);
			}

			GeneratorCommon& operator=(const GeneratorCommon&) = default;
			GeneratorCommon& operator=(GeneratorCommon&&) noexcept = default;
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
	class GeneratorOf final: public Private::GeneratorCommon
	{
		public:
			using TValue = TValue_;
			using iterator = Private::GeneratorOfIterator<GeneratorOf>;

		private:
			std::function<std::optional<TValue>(size_t)> m_Generator;
			std::optional<TValue> m_Value;

		protected:
			bool InvokeGenerator(size_t index) override
			{
				m_Value = std::invoke(m_Generator, index);
				return m_Value.has_value();
			}

		public:
			GeneratorOf() noexcept = default;

			template<class TFunc, std::enable_if_t<std::is_invocable_r_v<std::optional<TValue>, TFunc, size_t>, int> = 0>
			GeneratorOf(TFunc&& generator, size_t count = npos) noexcept
				:m_Generator(std::forward<TFunc>(generator)), GeneratorCommon(count)
			{
			}

			GeneratorOf(const GeneratorOf&) = default;
			GeneratorOf(GeneratorOf&&) noexcept = default;

		public:
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
	};
}
