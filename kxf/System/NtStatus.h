#pragma once
#include "Common.h"
#include "kxf/General/IErrorCode.h"

namespace kxf
{
	class HResult;
	class Win32Error;
}

namespace kxf
{
	class NtStatus final: public RTTI::ExtendInterface<NtStatus, IErrorCode>
	{
		KxRTTI_DeclareIID(NtStatus, {0xcffef908, 0x7112, 0x4130, {0x91, 0x8c, 0x6c, 0x2, 0xcb, 0x42, 0x94, 0xf1}});

		public:
			static NtStatus Success() noexcept;
			static NtStatus Fail() noexcept;

			static NtStatus GetLastError() noexcept;
			static void SetLastError(NtStatus error) noexcept;

		private:
			int32_t m_Value = 0;

		public:
			NtStatus(int32_t value) noexcept
				:m_Value(value)
			{
			}
			
		public:
			// IErrorCode
			bool IsSuccess() const noexcept override;
			bool IsFail() const noexcept override
			{
				return !IsSuccess();
			}

			uint32_t GetValue() const noexcept override
			{
				return m_Value;
			}
			void SetValue(uint32_t value) noexcept override
			{
				m_Value = value;
			}

			String ToString() const override;
			String GetMessage(const Locale& locale = {}) const override;

			// Win32Error
			bool IsError() const noexcept;
			bool IsWarning() const noexcept;
			bool IsInformation() const noexcept;
			uint32_t GetFacility() const noexcept;

			std::optional<Win32Error> ToWin32() const noexcept;
			std::optional<HResult> ToHResult() const noexcept;

		public:
			bool operator==(const NtStatus& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator!=(const NtStatus& other) const noexcept
			{
				return m_Value != other.m_Value;
			}
	};
}
