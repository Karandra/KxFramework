#pragma once
#include "Common.h"
#include "Kx/Utility/Common.h"

namespace KxFramework
{
	template<class T, T t_InitialValue = static_cast<T>(0)>
	class WithOptions
	{
		public:
			using TEnum = T;
			using TInt = std::underlying_type_t<TEnum>;

		private:
			TEnum m_Value = t_InitialValue;

		protected:
			constexpr WithOptions() noexcept = default;
			constexpr WithOptions(TEnum value) noexcept
				:m_Value(value)
			{
			}
			~WithOptions() noexcept = default;

		public:
			constexpr TEnum RawGetOptions() const noexcept
			{
				return m_Value;
			}
			constexpr void RawSetOptions(TEnum options) noexcept
			{
				m_Value = options;
			}
			constexpr void RawSetOptions(TInt options) noexcept
			{
				m_Value = static_cast<TEnum>(options);
			}

			constexpr bool IsOptionEnabled(TEnum option) const noexcept
			{
				return Utility::HasFlag(m_Value, option);
			}
			constexpr void SetOptionEnabled(TEnum option, bool enable = true) noexcept
			{
				Utility::ModFlagRef(m_Value, option, enable);
			}

		public:
			constexpr bool operator==(const WithOptions& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			constexpr bool operator!=(const WithOptions& other) const noexcept
			{
				return m_Value != other.m_Value;
			}
	};
}
