/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"

template<class T, T t_DefaultOptions = (T)0>
class KxOptionSet
{
	public:
		using TWithOptions = typename KxWithOptions<T, t_DefaultOptions>;
		using TEnum = typename TWithOptions::TEnum;
		using TInt = typename TWithOptions::TInt;

	private:
		TWithOptions m_Value;

	public:
		constexpr KxOptionSet() = default;
		constexpr KxOptionSet(KxOptionSet&& other) noexcept
		{
			*this = std::move(other);
		}
		constexpr KxOptionSet(const KxOptionSet& other) noexcept
			:m_Value(other.m_Value)
		{
		}
		constexpr KxOptionSet(const TWithOptions& withOptions) noexcept
			:m_Value(withOptions)
		{
		}
		constexpr KxOptionSet(TEnum value) noexcept
			:m_Value(value)
		{
		}

	public:
		constexpr TEnum GetValue() const noexcept
		{
			return m_Value.GetOptionsValue();
		}
		constexpr void SetValue(TEnum options) noexcept
		{
			m_Value.SetOptionsValue(options);
		}
		constexpr void SetValue(TInt options) noexcept
		{
			m_Value.SetOptionsValue(options);
		}

		constexpr bool IsEnabled(TEnum option) const noexcept
		{
			return m_Value.IsOptionEnabled(option);
		}
		constexpr void Enable(TEnum option, bool enable = true) noexcept
		{
			m_Value.SetOptionEnabled(option, enable);
		}
		constexpr void Disable(TEnum option) noexcept
		{
			m_Value.SetOptionEnabled(option, false);
		}

	public:
		constexpr bool operator==(const KxOptionSet& other) const noexcept
		{
			return m_Value == other.m_Value;
		}
		constexpr bool operator!=(const KxOptionSet& other) const noexcept
		{
			return !(*this == other);
		}

		constexpr KxOptionSet& operator=(KxOptionSet&& other) noexcept
		{
			m_Value = std::move(other.m_Value);
			return *this;
		}
		constexpr KxOptionSet& operator=(const KxOptionSet& other) noexcept
		{
			m_Value = other.m_Value;
			return *this;
		}

		constexpr operator TWithOptions() const
		{
			return m_Value;
		}
};
