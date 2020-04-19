#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include "Kx/Localization/Locale.h"
#include "Private/ErrorCodeValue.h"

namespace KxFramework
{
	class UniversallyUniqueID;
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
			static Win32Error GetLastError() noexcept;

		public:
			constexpr Win32Error(TValueType value) noexcept
				:ErrorCodeValue(value)
			{
			}
			
		public:
			bool IsSuccess() const noexcept;
			bool IsFail() const noexcept
			{
				return !IsSuccess();
			}

			String ToString() const;
			String GetMessage(const Locale& locale = {}) const;
	};

	class HResult final: public System::Private::ErrorCodeValue<HResult, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::HResult;
			}

		public:
			constexpr HResult(TValueType value) noexcept
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

			String ToString() const;
			String GetMessage(const Locale& locale = {}) const;

			String GetSource() const;
			String GetHelpFile() const;
			uint32_t GetHelpContext() const noexcept;
			String GetDescription() const;
			uint32_t GetFacility() const noexcept;
			UniversallyUniqueID GetUniqueID() const noexcept;
	};

	class NtStatus final: public System::Private::ErrorCodeValue<NtStatus, int32_t>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::NtStatus;
			}

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
	};
}
