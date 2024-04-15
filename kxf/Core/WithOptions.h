#pragma once
#include "Common.h"
#include "FlagSet.h"

namespace kxf::Private
{
	template<class TEnum>
	struct WithOptionsValue final
	{
		public:
			TEnum Value = static_cast<TEnum>(0);

		public:
			constexpr WithOptionsValue() noexcept = default;
			constexpr WithOptionsValue(TEnum value) noexcept
				:Value(value)
			{
			}
			constexpr WithOptionsValue(Private::FlagSetIntermediate<TEnum> value) noexcept
				:Value(value.Value)
			{
			}
	};
}

namespace kxf
{
	template<class TEnum, Private::WithOptionsValue<TEnum> initialValue = Private::WithOptionsValue<TEnum>()>
	class WithOptions
	{
		private:
			FlagSet<TEnum> m_Flags = initialValue.Value;

		protected:
			constexpr WithOptions() noexcept = default;
			constexpr WithOptions(FlagSet<TEnum> flags) noexcept
				:m_Flags(flags)
			{
			}

		public:
			constexpr FlagSet<TEnum> GetOptionFlags() const noexcept
			{
				return m_Flags;
			}
			constexpr WithOptions& SetOptionFlags(FlagSet<TEnum> option) noexcept
			{
				m_Flags = option;
				return *this;
			}

			constexpr bool ContainsOption(FlagSet<TEnum> option) const noexcept
			{
				return m_Flags.Contains(option);
			}
			constexpr WithOptions& AddOption(FlagSet<TEnum> option) noexcept
			{
				m_Flags.Add(option);
				return *this;
			}
			constexpr WithOptions& AddOption(FlagSet<TEnum> option, bool condition) noexcept
			{
				m_Flags.Add(option, condition);
				return *this;
			}
			constexpr WithOptions& RemoveOption(FlagSet<TEnum> option) noexcept
			{
				m_Flags.Remove(option);
				return *this;
			}
			constexpr WithOptions& RemoveOption(FlagSet<TEnum> option, bool condition) noexcept
			{
				m_Flags.Remove(option, condition);
				return *this;
			}
			constexpr WithOptions& ToggleOption(FlagSet<TEnum> option) noexcept
			{
				m_Flags.Toggle(option);
				return *this;
			}
			constexpr WithOptions& ModOption(FlagSet<TEnum> option, bool enable) noexcept
			{
				m_Flags.Mod(option, enable);
				return *this;
			}

		public:
			constexpr bool operator==(const WithOptions& other) const noexcept
			{
				return m_Flags == other.m_Flags;
			}
			constexpr bool operator!=(const WithOptions& other) const noexcept
			{
				return m_Flags != other.m_Flags;
			}
	};
}
