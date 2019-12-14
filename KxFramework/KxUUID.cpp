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
	int Compare(const ::UUID& left, const ::UUID& right) noexcept
	{
		RPC_STATUS status = RPC_S_OK;
		return ::UuidCompare(const_cast<::UUID*>(&left), const_cast<::UUID*>(&right), &status);
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
	if (SUCCEEDED(::CoCreateGuid(&m_ID)))
	{
		return KxUUIDStatus::OK;
	}
	return KxUUIDStatus::Unknown;
}
KxUUIDStatus KxUUID::CreateSequential() noexcept
{
	return RPCStatusToUUIDStatus(::UuidCreateSequential(&m_ID));
}
size_t KxUUID::GetHash() const noexcept
{
	static_assert(sizeof(m_ID) == 16, "UUID must be 16 bytes in size");

	if constexpr(sizeof(size_t) == sizeof(uint64_t))
	{
		const size_t* parts = reinterpret_cast<const size_t*>(&m_ID);
		return parts[0] ^ parts[1];
	}
	else if constexpr(sizeof(size_t) == sizeof(uint32_t))
	{
		const size_t* parts = reinterpret_cast<const size_t*>(&m_ID);
		return parts[0] ^ parts[1] ^ parts[2] ^ parts[3];
	}
	return 0;
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
bool KxUUID::operator<(const KxUUID& other) const noexcept
{
	return Compare(m_ID, other.m_ID) < 0;
}
bool KxUUID::operator<=(const KxUUID& other) const noexcept
{
	return Compare(m_ID, other.m_ID) <= 0;
}
bool KxUUID::operator>(const KxUUID& other) const noexcept
{
	return Compare(m_ID, other.m_ID) > 0;
}
bool KxUUID::operator>=(const KxUUID& other) const noexcept
{
	return Compare(m_ID, other.m_ID) >= 0;
}

bool KxUUID::operator==(const ::UUID& other) const noexcept
{
	return Compare(m_ID, other) == 0;
}
bool KxUUID::operator<(const ::UUID& other) const noexcept
{
	return Compare(m_ID, other) < 0;
}
bool KxUUID::operator<=(const ::UUID& other) const noexcept
{
	return Compare(m_ID, other) <= 0;
}
bool KxUUID::operator>(const ::UUID& other) const noexcept
{
	return Compare(m_ID, other) > 0;
}
bool KxUUID::operator>=(const ::UUID& other) const noexcept
{
	return Compare(m_ID, other) >= 0;
}
