#include "KxStdAfx.h"
#include "ErrorCode.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include <KxFramework/KxSystemAPI.h>
#include "IncludeNtStatus.h"

namespace
{
	// https://stackoverflow.com/questions/25566234/how-to-convert-specific-ntstatus-value-to-the-hresult

	std::optional<DWORD> Win32FromNtStatus(NTSTATUS ntStatus) noexcept
	{
		using namespace KxFramework;

		if (ntStatus == STATUS_SUCCESS)
		{
			return ERROR_SUCCESS;
		}
		else if (KxSystemAPI::RtlNtStatusToDosError)
		{
			const ULONG win32Code = KxSystemAPI::RtlNtStatusToDosError(ntStatus);
			if (win32Code != ERROR_MR_MID_NOT_FOUND)
			{
				return win32Code;
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
			return result;
		}
		return {};
	}
	std::optional<DWORD> Win32FromHRESULT(HRESULT hresult) noexcept
	{
		if (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0) == static_cast<HRESULT>(hresult & 0xFFFF0000))
		{
			// Could have come from many values, but we choose this one
			return HRESULT_CODE(hresult);
		}
		else if (hresult == S_OK)
		{
			return HRESULT_CODE(hresult);
		}
		else
		{
			// Otherwise, we got an impossible value
			return {};
		}
	}
	std::optional<DWORD> NtStatusFromWin32(DWORD win32Code) noexcept
	{
		switch (win32Code)
		{
			#include "ErrorCodeNtStatus.i"
		};
		return {};
	}
}

namespace KxFramework
{
	std::optional<int64_t> ErrorCode::ConvertToWin32() const noexcept
	{
		if (m_Value)
		{
			switch (m_Category)
			{
				case ErrorCodeCategory::Win32:
				{
					return m_Value;
				}
				case ErrorCodeCategory::NtStatus:
				{
					return Win32FromNtStatus(*m_Value);
				}
				case ErrorCodeCategory::HRESULT:
				{
					return Win32FromHRESULT(*m_Value);
				}
			};
		}
		return {};
	}
	std::optional<int64_t> ErrorCode::ConvertToNtStatus() const noexcept
	{
		if (m_Value)
		{
			switch (m_Category)
			{
				case ErrorCodeCategory::Win32:
				{
					return NtStatusFromWin32(*m_Value);
				}
				case ErrorCodeCategory::NtStatus:
				{
					return m_Value;
				}
				case ErrorCodeCategory::HRESULT:
				{
					if (auto win32Code = Win32FromHRESULT(*m_Value))
					{
						return NtStatusFromWin32(*win32Code);
					}
					break;
				}
			};
		}
		return {};
	}
	std::optional<int64_t> ErrorCode::ConvertToHRESULT() const noexcept
	{
		if (m_Value)
		{
			switch (m_Category)
			{
				case ErrorCodeCategory::Win32:
				{
					return HRESULT_FROM_WIN32(*m_Value);
				}
				case ErrorCodeCategory::NtStatus:
				{
					if (auto ntStatus = Win32FromNtStatus(*m_Value))
					{
						return HRESULT_FROM_WIN32(*ntStatus);
					}
					break;
				}
				case ErrorCodeCategory::HRESULT:
				{
					return m_Value;
				}
			};
		}
		return {};
	}

	bool ErrorCode::IsSuccessful() const noexcept
	{
		if (m_Value)
		{
			switch (m_Category)
			{
				case ErrorCodeCategory::Unknown:
				{
					// We can not know this for unknown error category
					return false;
				}
				case ErrorCodeCategory::Generic:
				{
					// Generic error codes use zero as the success (as most error codes do)
					return *m_Value == 0;
				}
				case ErrorCodeCategory::Win32:
				{
					return static_cast<DWORD>(*m_Value) == ERROR_SUCCESS;
				}
				case ErrorCodeCategory::NtStatus:
				{
					return static_cast<NTSTATUS>(*m_Value) == STATUS_SUCCESS;
				}
				case ErrorCodeCategory::HRESULT:
				{
					// HRESULT code considered as successful when it's greater than or equal to zero.
					return SUCCEEDED(static_cast<HRESULT>(*m_Value));
				}
			};
		}
		return false;
	}
	bool ErrorCode::IsFailed() const noexcept
	{
		return m_Value && m_Category != ErrorCodeCategory::Unknown && !IsSuccessful();
	}
}
