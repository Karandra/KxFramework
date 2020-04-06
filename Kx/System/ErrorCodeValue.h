#pragma once
#include "Common.h"

namespace KxFramework
{
	class UniversallyUniqueID;
}

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
			constexpr operator TValueType() const noexcept
			{
				return GetValue();
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
			constexpr GenericErrorCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			constexpr bool IsSuccess() const noexcept
			{
				return GetValue() == 0;
			}
			constexpr bool IsFail() const noexcept
			{
				return !IsSuccess();
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
			constexpr Win32ErrorCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsSuccess() const noexcept;
			bool IsFail() const noexcept
			{
				return !IsSuccess();
			}

			wxString ToString() const;
			wxString GetMessage() const;
	};

	class HResultCode final: public System::ErrorCodeValue<HResultCode, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::HResult;
			}

		public:
			constexpr HResultCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsOK() const noexcept;
			bool IsFalse() const noexcept;
			bool IsSuccess() const noexcept;
			bool IsFail() const noexcept
			{
				return !IsSuccess();
			}

			wxString ToString() const;
			wxString GetMessage() const;

			wxString GetSource() const;
			wxString GetHelpFile() const;
			uint32_t GetHelpContext() const noexcept;
			wxString GetDescription() const;
			uint32_t GetFacility() const noexcept;
			UniversallyUniqueID GetUniqueID() const noexcept;
	};

	class NtStatusCode final: public System::ErrorCodeValue<NtStatusCode, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::NtStatus;
			}

		public:
			constexpr NtStatusCode(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsError() const noexcept;
			bool IsWarning() const noexcept;
			bool IsInformation() const noexcept;
			bool IsSuccess() const noexcept;
			bool IsFailed() const noexcept
			{
				return !IsSuccess();
			}

			wxString ToString() const;
			wxString GetMessage() const;
			uint32_t GetFacility() const noexcept;
	};
}
