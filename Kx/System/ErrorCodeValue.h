#pragma once
#include "Common.h"

namespace KxFramework::System
{
	template<class TDerived, class T>
	class ErrorCodeValue
	{
		public:
			using TValueType = T;

		private:
			TValueType m_Value = 0;

		private:
			const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		protected:
			ErrorCodeValue(TValueType value) noexcept
				:m_Value(value)
			{
				static_assert(std::is_integral_v<T> || std::is_enum_v<T>, "invalid error code type");
			}

		public:
			TValueType GetValue() const noexcept
			{
				return m_Value;
			}
			ErrorCodeValue& SetValue(TValueType value) noexcept
			{
				m_Value = value;
				return *this;
			}
			operator TValueType() const noexcept
			{
				return GetValue();
			}
			
		public:
			explicit operator bool() const noexcept
			{
				return Self().IsSuccessful();
			}
			bool operator!() const noexcept
			{
				return !Self().IsSuccessful();
			}
	};
}

namespace KxFramework
{
	class GenericErrorCode final: public System::ErrorCodeValue<GenericErrorCode, uint32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::Generic;
			}

		public:
			explicit GenericErrorCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsSuccessful() const noexcept
			{
				return GetValue() == 0;
			}
			bool IsFailed() const noexcept
			{
				return !IsSuccessful();
			}
	};

	class Win32ErrorCode final: public System::ErrorCodeValue<Win32ErrorCode, uint32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::Win32;
			}

		public:
			explicit Win32ErrorCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsSuccessful() const noexcept;
			bool IsFailed() const noexcept
			{
				return !IsSuccessful();
			}
	};

	class NtStatusCode final: public System::ErrorCodeValue<NtStatusCode, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::NtStatus;
			}

		public:
			explicit NtStatusCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsSuccessful() const noexcept;
			bool IsFailed() const noexcept
			{
				return !IsSuccessful();
			}
	};

	class HResultCode final: public System::ErrorCodeValue<HResultCode, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::HResult;
			}

		public:
			explicit HResultCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsSuccessful() const noexcept;
			bool IsFailed() const noexcept
			{
				return !IsSuccessful();
			}
	};
}
