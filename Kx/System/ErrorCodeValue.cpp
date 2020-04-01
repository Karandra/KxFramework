#include "KxStdAfx.h"
#include "ErrorCodeValue.h"
#include "IncludeNtStatus.h"

namespace KxFramework
{
	bool Win32ErrorCode::IsSuccessful() const noexcept
	{
		return GetValue() == ERROR_SUCCESS;
	}
	bool NtStatusCode::IsSuccessful() const noexcept
	{
		return GetValue() == STATUS_SUCCESS;
	}
	bool HRESULTCode::IsSuccessful() const noexcept
	{
		return SUCCEEDED(GetValue());
	}
}
