#pragma once
#include "Common.h"
#include "Kx/Utility/Common.h"

namespace KxFramework
{
	template<class T, T t_InitialValue = static_cast<T>(0)>
	class OptionSet
	{
		public:
			using TEnum = T;
			using TInt = std::underlying_type_t<TEnum>;

		private:
			TEnum m_Value = t_InitialValue;

		public:
			constexpr OptionSet() = default;
			constexpr OptionSet(TEnum value) noexcept
				:m_Value(value)
			{
			}

		public:
			constexpr TEnum RawGetValue() const noexcept
			{
				return m_Value;
			}
			constexpr void RawSetValue(TEnum options) noexcept
			{
				m_Value = options;
			}
			constexpr void RawSetValue(TInt options) noexcept
			{
				m_Value = static_cast<TEnum>(options);
			}

			constexpr bool IsEnabled(TEnum option) const noexcept
			{
				return Utility::HasFlag(m_Value, option);
			}
			constexpr void Enable(TEnum option, bool enable = true) noexcept
			{
				Utility::ModFlagRef(m_Value, option, enable);
			}
			constexpr void Disable(TEnum option) noexcept
			{
				Utility::RemoveFlagRef(m_Value, option);
			}

		public:
			constexpr bool operator==(const OptionSet& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			constexpr bool operator!=(const OptionSet& other) const noexcept
			{
				return !(*this == other);
			}
	};
}
