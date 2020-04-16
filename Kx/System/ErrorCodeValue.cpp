#include "stdafx.h"
#include "ErrorCodeValue.h"
#include "Kx/General/UniversallyUniqueID.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include "Private/IncludeNtStatus.h"

#include "Private/BeginIncludeCOM.h"
namespace
{
	Kx_MakeWinUnicodeCallWrapper(FormatMessage);
}
#include <comdef.h>
#include "Private/EndIncludeCOM.h"

namespace
{
	wxString FormatSystemMessage(uint32_t flags, const void* source, uint32_t messageID, uint32_t langID)
	{
		using namespace KxFramework;

		wchar_t* formattedMessage = nullptr;
		uint32_t length = ::FormatMessageW(flags|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,
										   source,
										   messageID,
										   MAKELCID(langID, SORT_DEFAULT),
										   reinterpret_cast<LPWSTR>(&formattedMessage),
										   0,
										   nullptr
		);
		if (length != 0 && formattedMessage)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				::LocalFree(formattedMessage);
			});
			return wxString(formattedMessage, length);
		}
		return {};
	}

	// Inclusion of 'NtDef.h' causes too much compile errors
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
}

namespace KxFramework
{
	Win32Error Win32Error::GetLastError() noexcept
	{
		return ::GetLastError();
	}

	bool Win32Error::IsSuccess() const noexcept
	{
		return GetValue() == ERROR_SUCCESS;
	}

	String Win32Error::ToString() const
	{
		return {};
	}
	String Win32Error::GetMessage() const
	{
		return FormatSystemMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, GetValue(), 0);
	}
}

namespace KxFramework
{
	bool HResult::IsOK() const noexcept
	{
		return GetValue() == S_OK;
	}
	bool HResult::IsFalse() const noexcept
	{
		return GetValue() == S_FALSE;
	}
	bool HResult::IsSuccess() const noexcept
	{
		return SUCCEEDED(GetValue());
	}

	String HResult::ToString() const
	{
		return {};
	}
	String HResult::GetMessage() const
	{
		return _com_error(GetValue()).ErrorMessage();
	}

	String HResult::GetSource() const
	{
		_bstr_t result = _com_error(GetValue()).Source();
		return static_cast<const wxChar*>(result);
	}
	String HResult::GetHelpFile() const
	{
		_bstr_t result = _com_error(GetValue()).HelpFile();
		return static_cast<const wxChar*>(result);
	}
	uint32_t HResult::GetHelpContext() const noexcept
	{
		return _com_error(GetValue()).HelpContext();
	}
	String HResult::GetDescription() const
	{
		_bstr_t result = _com_error(GetValue()).Description();
		return static_cast<const wxChar*>(result);
	}
	uint32_t HResult::GetFacility() const noexcept
	{
		return HRESULT_FACILITY(static_cast<HRESULT>(GetValue()));
	}
	UniversallyUniqueID HResult::GetUniqueID() const noexcept
	{
		GUID result = _com_error(GetValue()).GUID();
		return *reinterpret_cast<NativeUUID*>(&result);
	}
}

namespace KxFramework
{
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
	String NtStatus::GetMessage() const
	{
		return FormatSystemMessage(FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_MAX_WIDTH_MASK, ::GetModuleHandleW(L"NtDLL.dll"), GetValue(), 0);
	}
	uint32_t NtStatus::GetFacility() const noexcept
	{
		return NT_FACILITY(GetValue());
	}
}
