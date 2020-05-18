#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include "Kx/Localization/Locale.h"
#include "Private/ErrorCodeValue.h"
#include "UndefWindows.h"
struct IErrorInfo;

namespace KxFramework
{
	class UniversallyUniqueID;

	class Win32Error;
	class HResult;
	class NtStatus;
}

namespace KxFramework
{
	class GenericError final: public System::Private::ErrorCodeValue<GenericError, uint32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::Generic;
			}
			
			constexpr static GenericError Success() noexcept
			{
				return 0;
			}
			constexpr static GenericError Fail() noexcept
			{
				return std::numeric_limits<TValueType>::max();
			}

		public:
			constexpr GenericError(TValueType value) noexcept
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

			String ToString() const
			{
				return {};
			}
			String GetMessage(const Locale& locale = {}) const
			{
				return {};
			}
	};

	class Win32Error final: public System::Private::ErrorCodeValue<Win32Error, uint32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::Win32;
			}
			
			static Win32Error Success() noexcept;
			static Win32Error Fail() noexcept;

			static Win32Error GetLastError() noexcept;
			static void SetLastError(Win32Error error) noexcept;

		public:
			constexpr Win32Error(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsSuccess() const noexcept
			{
				return *this == Success();
			}
			bool IsFail() const noexcept
			{
				return !IsSuccess();
			}

			String ToString() const;
			String GetMessage(const Locale& locale = {}) const;

			std::optional<HResult> ToHResult() const noexcept;
			std::optional<NtStatus> ToNtStatus() const noexcept;
	};

	class HResult final: public System::Private::ErrorCodeValue<HResult, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::HResult;
			}

			static HResult Success() noexcept;
			static HResult False() noexcept;
			static HResult Fail() noexcept;

		private:
			IErrorInfo* m_ErrorInfo = nullptr;

		public:
			constexpr HResult(TValueType value, IErrorInfo* errorInfo = nullptr) noexcept
				:ErrorCodeValue(value), m_ErrorInfo(errorInfo)
			{
			}
			
		public:
			bool IsOK() const noexcept
			{
				return *this == Success();
			}
			bool IsFalse() const noexcept
			{
				return *this == False();
			}
			bool IsSuccess() const noexcept;
			bool IsFail() const noexcept
			{
				return !IsSuccess();
			}

			String ToString() const;
			String GetMessage(const Locale& locale = {}) const;

			String GetSource() const;
			String GetHelpFile() const;
			uint32_t GetHelpContext() const noexcept;
			String GetDescription() const;
			uint32_t GetFacility() const noexcept;
			UniversallyUniqueID GetUniqueID() const noexcept;

			std::optional<Win32Error> ToWin32() const noexcept;
			std::optional<NtStatus> ToNtStatus() const noexcept;
	};

	class NtStatus final: public System::Private::ErrorCodeValue<NtStatus, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::NtStatus;
			}

			static NtStatus Success() noexcept;
			static NtStatus Fail() noexcept;

		public:
			constexpr NtStatus(TValueType value) noexcept
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

			String ToString() const;
			String GetMessage(const Locale& locale = {}) const;
			uint32_t GetFacility() const noexcept;

			std::optional<Win32Error> ToWin32() const noexcept;
			std::optional<HResult> ToHResult() const noexcept;
	};
}
