#include "stdafx.h"
#include "ErrorCode.h"

namespace KxFramework
{
	std::optional<Win32Error> ErrorCode::ConvertToWin32() const noexcept
	{
		switch (m_Category)
		{
			case ErrorCodeCategory::Generic:
			{
				return GenericError(m_Value) ? Win32Error::Success() : Win32Error::Fail();
			}
			case ErrorCodeCategory::Win32:
			{
				return Win32Error(m_Value);
			}
			case ErrorCodeCategory::NtStatus:
			{
				return NtStatus(m_Value).ToWin32();
			}
			case ErrorCodeCategory::HResult:
			{
				return HResult(m_Value).ToWin32();
			}
		};
		return {};
	}
	std::optional<NtStatus> ErrorCode::ConvertToNtStatus() const noexcept
	{
		switch (m_Category)
		{
			case ErrorCodeCategory::Generic:
			{
				return GenericError(m_Value) ? NtStatus::Success() : NtStatus::Fail();
			}
			case ErrorCodeCategory::Win32:
			{
				return Win32Error(m_Value).ToNtStatus();
			}
			case ErrorCodeCategory::NtStatus:
			{
				return NtStatus(m_Value);
			}
			case ErrorCodeCategory::HResult:
			{
				return HResult(m_Value).ToNtStatus();
			}
		};
		return {};
	}
	std::optional<HResult> ErrorCode::ConvertToHResult() const noexcept
	{
		switch (m_Category)
		{
			case ErrorCodeCategory::Generic:
			{
				return GenericError(m_Value) ? HResult::Success() : HResult::Fail();
			}
			case ErrorCodeCategory::Win32:
			{
				return Win32Error(m_Value).ToHResult();
			}
			case ErrorCodeCategory::NtStatus:
			{
				return NtStatus(m_Value).ToHResult();
			}
			case ErrorCodeCategory::HResult:
			{
				return HResult(m_Value);
			}
		};
		return {};
	}
}
