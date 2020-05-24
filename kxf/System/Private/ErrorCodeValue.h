#pragma once
#include "../Common.h"

namespace kxf::System::Private
{
	template<class TDerived, class T>
	class ErrorCodeValue
	{
		public:
			using TValueType = T;

		private:
			TValueType m_Value = 0;

		private:
			constexpr const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		protected:
			constexpr ErrorCodeValue(TValueType value) noexcept
				:m_Value(value)
			{
				static_assert(std::is_integral_v<T> || std::is_enum_v<T>, "invalid error code type");
			}

		public:
			constexpr TValueType GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr ErrorCodeValue& SetValue(TValueType value) noexcept
			{
				m_Value = value;
				return *this;
			}
			
		public:
			constexpr explicit operator bool() const noexcept
			{
				return Self().IsSuccess();
			}
			constexpr bool operator!() const noexcept
			{
				return Self().IsFail();
			}

			constexpr bool operator==(const ErrorCodeValue& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			constexpr bool operator!=(const ErrorCodeValue& other) const noexcept
			{
				return !(*this == other);
			}

			constexpr TValueType operator*() const noexcept
			{
				return GetValue();
			}
	};
}
