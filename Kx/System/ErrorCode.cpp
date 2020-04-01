#include "KxStdAfx.h"
#include "ErrorCode.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include <KxFramework/KxSystemAPI.h>
#include "IncludeNtStatus.h"

namespace
{
	std::optional<KxFramework::Win32ErrorCode> Win32FromNtStatus(NTSTATUS ntStatus) noexcept
	{
		// https://stackoverflow.com/questions/25566234/how-to-convert-specific-ntstatus-value-to-the-hresult
		using namespace KxFramework;

		if (ntStatus == STATUS_SUCCESS)
		{
			return Win32ErrorCode(ERROR_SUCCESS);
		}
		else if (KxSystemAPI::RtlNtStatusToDosError)
		{
			const ULONG win32Code = KxSystemAPI::RtlNtStatusToDosError(ntStatus);
			if (win32Code != ERROR_MR_MID_NOT_FOUND)
			{
				return Win32ErrorCode(win32Code);
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
			return Win32ErrorCode(result);
		}
		return {};
	}
	std::optional<KxFramework::Win32ErrorCode> Win32FromHRESULT(HRESULT hresult) noexcept
	{
		using namespace KxFramework;

		if (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0) == static_cast<HRESULT>(hresult & 0xFFFF0000))
		{
			// Could have come from many values, but we choose this one
			return Win32ErrorCode(HRESULT_CODE(hresult));
		}
		else if (hresult == S_OK)
		{
			return Win32ErrorCode(HRESULT_CODE(hresult));
		}
		else
		{
			// Otherwise, we got an impossible value
			return {};
		}
	}
	std::optional<KxFramework::NtStatusCode> NtStatusFromWin32(DWORD win32Code) noexcept
	{
		using namespace KxFramework;

		auto ntStatus = [win32Code]() -> std::optional<NTSTATUS>
		{
			switch (win32Code)
			{
				#include "ErrorCodeNtStatus.i"
			};
			return {};
		}();

		if (ntStatus)
		{
			return NtStatusCode(*ntStatus);
		}
		return {};
	}
}

namespace KxFramework
{
	std::optional<Win32ErrorCode> ErrorCode::ConvertToWin32() const noexcept
	{
		switch (m_Category)
		{
			case ErrorCodeCategory::Generic:
			{
				if (GenericErrorCode(m_Value).IsSuccessful())
				{
					return Win32ErrorCode(ERROR_SUCCESS);
				}
				break;
			}
			case ErrorCodeCategory::Win32:
			{
				return Win32ErrorCode(m_Value);
			}
			case ErrorCodeCategory::NtStatus:
			{
				return Win32FromNtStatus(m_Value);
			}
			case ErrorCodeCategory::HRESULT:
			{
				return Win32FromHRESULT(m_Value);
			}
		};
		return {};
	}
	std::optional<NtStatusCode> ErrorCode::ConvertToNtStatus() const noexcept
	{
		switch (m_Category)
		{
			case ErrorCodeCategory::Generic:
			{
				if (GenericErrorCode(m_Value).IsSuccessful())
				{
					return NtStatusCode(STATUS_SUCCESS);
				}
				break;
			}
			case ErrorCodeCategory::Win32:
			{
				return NtStatusFromWin32(m_Value);
			}
			case ErrorCodeCategory::NtStatus:
			{
				return NtStatusCode(m_Value);
			}
			case ErrorCodeCategory::HRESULT:
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
	std::optional<HRESULTCode> ErrorCode::ConvertToHRESULT() const noexcept
	{
		switch (m_Category)
		{
			case ErrorCodeCategory::Generic:
			{
				if (GenericErrorCode(m_Value).IsSuccessful())
				{
					return HRESULTCode(S_OK);
				}
				break;
			}
			case ErrorCodeCategory::Win32:
			{
				return HRESULTCode(HRESULT_FROM_WIN32(m_Value));
			}
			case ErrorCodeCategory::NtStatus:
			{
				if (auto ntStatus = Win32FromNtStatus(m_Value))
				{
					return HRESULTCode(HRESULT_FROM_WIN32(*ntStatus));
				}
				break;
			}
			case ErrorCodeCategory::HRESULT:
			{
				return HRESULTCode(m_Value);
			}
		};
		return {};
	}
}
