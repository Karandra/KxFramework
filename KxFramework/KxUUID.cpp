/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxUUID.h"

namespace
{
	KxUUIDStatus RPCStatusToUUIDStatus(RPC_STATUS value) noexcept
	{
		switch (value)
		{
			case RPC_S_OK:
			{
				return KxUUIDStatus::OK;
			}
			case RPC_S_UUID_LOCAL_ONLY:
			{
				return KxUUIDStatus::LocalOnly;
			}
			case RPC_S_UUID_NO_ADDRESS:
			{
				return KxUUIDStatus::NoAddress;
			}
		};
		return KxUUIDStatus::Unknown;
	}
	int Compare(const ::UUID& v1, const ::UUID& v2) noexcept
	{
		RPC_STATUS status = RPC_S_OK;
		return ::UuidCompare(const_cast<::UUID*>(&v1), const_cast<::UUID*>(&v2), &status);
	}
}

KxUUID::KxUUID() noexcept
{
	::UuidCreateNil(&m_ID);
}

bool KxUUID::IsNull() const noexcept
{
	RPC_STATUS status = RPC_S_OK;
	return ::UuidIsNil(const_cast<::UUID*>(&m_ID), &status) == TRUE;
}
KxUUIDStatus KxUUID::Create() noexcept
{
	return RPCStatusToUUIDStatus(::UuidCreate(&m_ID));
}

wxString KxUUID::ToString() const
{
	wchar_t* stringUUID = nullptr;
	::UuidToStringW(&m_ID, (RPC_WSTR*)&stringUUID);
	if (stringUUID)
	{
		wxString copy(stringUUID);
		::RpcStringFreeW((RPC_WSTR*)&stringUUID);
		return copy;
	}
	return {};
}
KxUUIDStatus KxUUID::FromString(const char* value) noexcept
{
	return RPCStatusToUUIDStatus(::UuidFromStringA(reinterpret_cast<RPC_CSTR>(const_cast<char*>(value)), &m_ID));
}
KxUUIDStatus KxUUID::FromString(const wchar_t* value) noexcept
{
	return RPCStatusToUUIDStatus(::UuidFromStringW(reinterpret_cast<RPC_WSTR>(const_cast<wchar_t*>(value)), &m_ID));
}

bool KxUUID::operator==(const KxUUID& other) const noexcept
{
	return Compare(m_ID, other.m_ID) == 0;
}
bool KxUUID::operator==(const ::UUID& other) const noexcept
{
	return Compare(m_ID, other) == 0;
}
