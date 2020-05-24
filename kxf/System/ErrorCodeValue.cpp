#include "stdafx.h"
#include "ErrorCodeValue.h"
#include "NativeAPI.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/Utility/CallAtScopeExit.h"

#include <Windows.h>
#include "Private/IncludeNtStatus.h"
#include "Private/System.h"

#include "Private/BeginIncludeCOM.h"
namespace
{
	Kx_MakeWinUnicodeCallWrapper(FormatMessage);
}
#include <comdef.h>
#include "Private/EndIncludeCOM.h"

#include "UndefWindows.h"

namespace
{
	// Inclusion of 'NtDef.h' causes too many compilation errors
	bool NT_SUCCESS(NTSTATUS status)
	{
		return status >= 0;
	}
	bool NT_INFORMATION(NTSTATUS status)
	{
		return static_cast<ULONG>(status >> 30) == STATUS_SEVERITY_INFORMATIONAL;
	}
	bool NT_WARNING(NTSTATUS status)
	{
		return static_cast<ULONG>(status >> 30) == STATUS_SEVERITY_WARNING;
	}
	bool NT_ERROR(NTSTATUS status)
	{
		return static_cast<ULONG>(status >> 30) == STATUS_SEVERITY_ERROR;
	}
	uint32_t NT_FACILITY(NTSTATUS status)
	{
		return status & 0xFFF0000u;
	}

	std::optional<kxf::Win32Error> Win32FromNtStatus(NTSTATUS ntStatus) noexcept
	{
		// https://stackoverflow.com/questions/25566234/how-to-convert-specific-ntstatus-value-to-the-hresult
		using namespace kxf;

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
		Utility::CallAtScopeExit atExit = ([previousError = ::GetLastError()]()
		{
			::SetLastError(previousError);
		});
		::SetLastError(ERROR_SUCCESS);

		DWORD bytes = 0;
		::GetOverlappedResult(nullptr, &overlapped, &bytes, FALSE);

		const DWORD result = ::GetLastError();
		if (result != ERROR_SUCCESS)
		{
			return Win32Error(result);
		}
		return {};
	}
	std::optional<kxf::Win32Error> Win32FromHRESULT(HRESULT hresult) noexcept
	{
		using namespace kxf;

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
	std::optional<kxf::NtStatus> NtStatusFromWin32(DWORD win32Code) noexcept
	{
		using namespace kxf;

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

namespace kxf
{
	Win32Error Win32Error::Success() noexcept
	{
		return ERROR_SUCCESS;
	}
	Win32Error Win32Error::Fail() noexcept
	{
		// There isn't any generic fail code but this should suffice
		return std::numeric_limits<TValueType>::max();
	}

	Win32Error Win32Error::GetLastError() noexcept
	{
		return ::GetLastError();
	}
	void Win32Error::SetLastError(Win32Error error) noexcept
	{
		::SetLastError(*error);
	}

	String Win32Error::ToString() const
	{
		return {};
	}
	String Win32Error::GetMessage(const Locale& locale) const
	{
		return System::Private::FormatMessage(nullptr, GetValue(), FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_MAX_WIDTH_MASK, locale);
	}

	std::optional<HResult> Win32Error::ToHResult() const noexcept
	{
		return HRESULT_FROM_WIN32(GetValue());
	}
	std::optional<NtStatus> Win32Error::ToNtStatus() const noexcept
	{
		return NtStatusFromWin32(GetValue());
	}
}

namespace kxf
{
	HResult HResult::Success() noexcept
	{
		return S_OK;
	}
	HResult HResult::False() noexcept
	{
		return S_FALSE;
	}
	HResult HResult::Fail() noexcept
	{
		return E_FAIL;
	}

	bool HResult::IsSuccess() const noexcept
	{
		return SUCCEEDED(GetValue());
	}

	String HResult::ToString() const
	{
		return {};
	}
	String HResult::GetMessage(const Locale& locale) const
	{
		return _com_error(GetValue(), m_ErrorInfo).ErrorMessage();
	}

	String HResult::GetSource() const
	{
		_bstr_t result = _com_error(GetValue(), m_ErrorInfo).Source();
		return static_cast<const wxChar*>(result);
	}
	String HResult::GetHelpFile() const
	{
		_bstr_t result = _com_error(GetValue(), m_ErrorInfo).HelpFile();
		return static_cast<const wxChar*>(result);
	}
	uint32_t HResult::GetHelpContext() const noexcept
	{
		return _com_error(GetValue(), m_ErrorInfo).HelpContext();
	}
	String HResult::GetDescription() const
	{
		_bstr_t result = _com_error(GetValue(), m_ErrorInfo).Description();
		return static_cast<const wxChar*>(result);
	}
	uint32_t HResult::GetFacility() const noexcept
	{
		return HRESULT_FACILITY(static_cast<HRESULT>(GetValue()));
	}
	UniversallyUniqueID HResult::GetUniqueID() const noexcept
	{
		GUID result = _com_error(GetValue(), m_ErrorInfo).GUID();
		return *reinterpret_cast<NativeUUID*>(&result);
	}

	std::optional<Win32Error> HResult::ToWin32() const noexcept
	{
		return Win32FromHRESULT(GetValue());
	}
	std::optional<NtStatus> HResult::ToNtStatus() const noexcept
	{
		if (auto win32Code = Win32FromHRESULT(GetValue()))
		{
			return NtStatusFromWin32(win32Code->GetValue());
		}
		return {};
	}
}

namespace kxf
{
	NtStatus NtStatus::Success() noexcept
	{
		return STATUS_SUCCESS;
	}
	NtStatus NtStatus::Fail() noexcept
	{
		// Same as with Win32 codes, there isn't any generic fail code for NtStatus
		return std::numeric_limits<TValueType>::max();
	}

	bool NtStatus::IsError() const noexcept
	{
		return NT_ERROR(GetValue());
	}
	bool NtStatus::IsWarning() const noexcept
	{
		return NT_WARNING(GetValue());
	}
	bool NtStatus::IsInformation() const noexcept
	{
		return NT_INFORMATION(GetValue());
	}
	bool NtStatus::IsSuccess() const noexcept
	{
		return NT_SUCCESS(GetValue());
	}

	String NtStatus::ToString() const
	{
		return {};
	}
	String NtStatus::GetMessage(const Locale& locale) const
	{
		return System::Private::FormatMessage(::GetModuleHandleW(L"NtDLL.dll"), GetValue(), FORMAT_MESSAGE_FROM_HMODULE, locale);
	}
	uint32_t NtStatus::GetFacility() const noexcept
	{
		return NT_FACILITY(GetValue());
	}

	std::optional<Win32Error> NtStatus::ToWin32() const noexcept
	{
		return Win32FromNtStatus(GetValue());
	}
	std::optional<HResult> NtStatus::ToHResult() const noexcept
	{
		if (auto win32 = Win32FromNtStatus(GetValue()))
		{
			return HRESULT_FROM_WIN32(win32->GetValue());
		}
		return {};
	}
}
