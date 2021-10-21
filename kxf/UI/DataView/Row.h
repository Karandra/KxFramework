#pragma once
#include "Common.h"

namespace kxf::DataView
{
	class Row final
	{
		private:
			static constexpr auto npos = std::numeric_limits<size_t>::max();

		private:
			size_t m_Value = npos;

		private:
			constexpr bool IsNull() const noexcept
			{
				return m_Value == npos;
			}

		public:
			constexpr Row(size_t value = npos) noexcept
				:m_Value(value)
			{
			}

		public:
			constexpr size_t GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr void SetValue(size_t value) noexcept
			{
				m_Value = value;
			}
			constexpr size_t operator*() const noexcept
			{
				return m_Value;
			}

		public:
			constexpr explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

			constexpr auto operator<=>(const Row&) const noexcept = default;
	};
}
