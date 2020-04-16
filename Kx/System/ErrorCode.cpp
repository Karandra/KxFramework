#include "KxStdAfx.h"
#include "ErrorCode.h"
#include "NativeAPI.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include "Private/IncludeNtStatus.h"

namespace
{
	std::optional<KxFramework::Win32Error> Win32FromNtStatus(NTSTATUS ntStatus) noexcept
	{
		// https://stackoverflow.com/questions/25566234/how-to-convert-specific-ntstatus-value-to-the-hresult
		using namespace KxFramework;

		if (ntStatus == STATUS_SUCCESS)
		{
			return Win32Error(ERROR_SUCCESS);
		}
		else if (NativeAPI::NtDLL::RtlNtStatusToDosError)
		{
			const ULONG win32Code = NativeAPI::NtDLL::RtlNtStatusToDosError(ntStatus);
			if (win32Code != ERROR_MR_MID_NOT_FOUND)
			{
				return Win32Error(win32Code);
			}
		}

		// Using GetOverlappedResult hack
		OVERLAPPED overlapped = {};
		overlapped.Internal = ntStatus;
		overlapped.InternalHigh = 0;
		overlapped.Offset = 0;
		overlapped.OffsetHigh = 0;
		overlapped.hEvent = nullptr;

		// Remember current error and reset it
		const DWORD previousError = ::GetLastError();
		::SetLastError(ERROR_SUCCESS);
		Utility::CallAtScopeExit atExit([&]()
		{
			::SetLastError(previousError);
		});

		DWORD bytes = 0;
		::GetOverlappedResult(nullptr, &overlapped, &bytes, FALSE);

		const DWORD result = ::GetLastError();
		if (result != ERROR_SUCCESS)
		{
			return Win32Error(result);
		}
		return {};
	}
	std::optional<KxFramework::Win32Error> Win32FromHRESULT(HRESULT hresult) noexcept
	{
		using namespace KxFramework;

		if (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0) == static_cast<HRESULT>(hresult & 0xFFFF0000))
		{
			// Could have come from many values, but we choose this one
			return Win32Error(HRESULT_CODE(hresult));
		}
		else if (hresult == S_OK)
		{
			return Win32Error(HRESULT_CODE(hresult));
		}
		else
		{
			// Otherwise, we got an impossible value
			return {};
		}
	}
	std::optional<KxFramework::NtStatus> NtStatusFromWin32(DWORD win32Code) noexcept
	{
		using namespace KxFramework;

		auto ntStatus = [win32Code]() -> std::optional<NTSTATUS>
		{
			switch (win32Code)
			{
				#include "Private/ErrorCodeNtStatus.i"
			};
			return {};
		}();

		if (ntStatus)
		{
			return NtStatus(*ntStatus);
		}
		return {};
	}
}

namespace KxFramework
{
	std::optional<Win32Error> ErrorCode::ConvertToWin32() const noexcept
	{
		switch (m_Category)
		{
			case ErrorCodeCategory::Generic:
			{
				if (GenericError(m_Value).IsSuccess())
				{
					return Win32Error(ERROR_SUCCESS);
				}
				break;
			}
			case ErrorCodeCategory::Win32:
			{
				return Win32Error(m_Value);
			}
			case ErrorCodeCategory::NtStatus:
			{
				return Win32FromNtStatus(m_Value);
			}
			case ErrorCodeCategory::HResult:
			{
				return Win32FromHRESULT(m_Value);
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
				if (GenericError(m_Value).IsSuccess())
				{
					return NtStatus(STATUS_SUCCESS);
				}
				break;
			}
			case ErrorCodeCategory::Win32:
			{
				return NtStatusFromWin32(m_Value);
			}
			case ErrorCodeCategory::NtStatus:
			{
				return NtStatus(m_Value);
			}
			case ErrorCodeCategory::HResult:
			{
				if (auto win32Code = Win32FromHRESULT(m_Value))
				{
					return NtStatusFromWin32(*win32Code);
				}
				break;
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
				if (GenericError(m_Value).IsSuccess())
				{
					return HResult(S_OK);
				}
				break;
			}
			case ErrorCodeCategory::Win32:
			{
				return HResult(HRESULT_FROM_WIN32(m_Value));
			}
			case ErrorCodeCategory::NtStatus:
			{
				if (auto ntStatus = Win32FromNtStatus(m_Value))
				{
					return HResult(HRESULT_FROM_WIN32(*ntStatus));
				}
				break;
			}
			case ErrorCodeCategory::HResult:
			{
				return HResult(m_Value);
			}
		};
		return {};
	}
}
