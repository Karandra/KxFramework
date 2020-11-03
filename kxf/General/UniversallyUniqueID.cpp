#include "stdafx.h"
#include "UniversallyUniqueID.h"
#include "LocallyUniqueID.h"
#include "kxf/System/HResult.h"
#include "kxf/System/Win32Error.h"

#include <Windows.h>
#include <rpcdce.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	template<class T, class TUUID>
	auto CastAs(TUUID&& uuid)
	{
		if constexpr(std::is_const_v<std::remove_reference_t<TUUID>>)
		{
			return reinterpret_cast<const T*>(&uuid);
		}
		else
		{
			return reinterpret_cast<T*>(&uuid);
		}
	}

	template<class T>
	auto AsGUID(T&& uuid)
	{
		return CastAs<::GUID>(std::forward<T>(uuid));
	}

	template<class T>
	auto AsUUID(T&& uuid)
	{
		return CastAs<::UUID>(std::forward<T>(uuid));
	}

	int Compare(const NativeUUID& left, const NativeUUID& right) noexcept
	{
		RPC_STATUS status = RPC_S_OK;
		return ::UuidCompare(const_cast<::UUID*>(AsUUID(left)), const_cast<::UUID*>(AsUUID(right)), &status);
	}

	kxf::NativeUUID DoCreateFromString(const wchar_t* value) noexcept
	{
		NativeUUID uuid;
		if (::UuidFromStringW(reinterpret_cast<RPC_WSTR>(const_cast<wchar_t*>(value)), AsUUID(uuid)) == RPC_S_OK)
		{
			return uuid;
		}
		else if (HResult(::CLSIDFromString(value, AsUUID(uuid))))
		{
			return uuid;
		}
		return {};
	}
	kxf::NativeUUID DoCreateFromString(const char* value) noexcept
	{
		NativeUUID uuid;
		if (::UuidFromStringA(reinterpret_cast<RPC_CSTR>(const_cast<char*>(value)), AsUUID(uuid)) == RPC_S_OK)
		{
			return uuid;
		}
		else if (kxf::String temp = value; HResult(::CLSIDFromString(temp.wc_str(), AsUUID(uuid))))
		{
			return uuid;
		}
		return {};
	}
}

namespace kxf
{
	UniversallyUniqueID UniversallyUniqueID::Create() noexcept
	{
		NativeUUID uuid;
		if (HResult(::CoCreateGuid(AsGUID(uuid))))
		{
			return uuid;
		}
		return {};
	}
	UniversallyUniqueID UniversallyUniqueID::CreateSequential() noexcept
	{
		NativeUUID uuid;
		Win32Error error = ::UuidCreateSequential(AsGUID(uuid));
		if (error.IsSuccess() || *error == RPC_S_UUID_LOCAL_ONLY)
		{
			return uuid;
		}
		return {};
	}
	
	UniversallyUniqueID UniversallyUniqueID::CreateFromInt128(const uint8_t(&bytes)[16]) noexcept
	{
		NativeUUID uuid;
		std::memcpy(&uuid, bytes, std::size(bytes));
		return uuid;
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromInt128(uint64_t low, uint64_t high) noexcept
	{
		uint8_t bytes[16] = {};
		std::memcpy(&bytes, &low, sizeof(low));
		std::memcpy(&bytes + sizeof(low), &high, sizeof(high));

		return CreateFromInt128(bytes);
	}

	UniversallyUniqueID UniversallyUniqueID::CreateFromString(const char* value) noexcept
	{
		return DoCreateFromString(value);
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromString(const wchar_t* value) noexcept
	{
		return DoCreateFromString(value);
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromString(const String& value) noexcept
	{
		return DoCreateFromString(value.wx_str());
	}

	UniversallyUniqueID::UniversallyUniqueID(LocallyUniqueID other) noexcept
	{
		uint64_t vlaue = other.ToInt();
		std::memset(&m_ID, 0, sizeof(vlaue));
		std::memcpy(&m_ID, &vlaue, sizeof(vlaue));
	}

	String UniversallyUniqueID::ToString(FlagSet<UUIDFormat> format) const
	{
		String uuid = [&]() -> String
		{
			wchar_t* stringUUID = nullptr;
			if (::UuidToStringW(AsUUID(m_ID), reinterpret_cast<RPC_WSTR*>(&stringUUID)) == RPC_S_OK && stringUUID)
			{
				String temp = stringUUID;
				::RpcStringFreeW(reinterpret_cast<RPC_WSTR*>(&stringUUID));
				return temp;
			}
			return {};
		}();

		if (format & UUIDFormat::UpperCase)
		{
			uuid.MakeUpper();
		}

		if (format & UUIDFormat::URN)
		{
			uuid.Prepend(wxS("urn:uuid:"));
		}

		if (format & UUIDFormat::CurlyBraces)
		{
			uuid.Prepend(wxS('{'));
			uuid.Append(wxS('}'));
		}
		else if (format & UUIDFormat::Parentheses)
		{
			uuid.Prepend(wxS('('));
			uuid.Append(wxS(')'));
		}

		return uuid;
	}
	LocallyUniqueID UniversallyUniqueID::ToLocallyUniqueID() const noexcept
	{
		for (uint8_t d4: m_ID.Data4)
		{
			if (d4 != 0)
			{
				return {};
			}
		}

		uint64_t value = 0;
		std::memcpy(&value, &m_ID, sizeof(value));
		return value;
	}
	std::array<uint8_t, 16> UniversallyUniqueID::ToInt128() const noexcept
	{
		std::array<uint8_t, 16> i128;
		std::memcpy(i128.data(), &m_ID, std::size(i128));
		return i128;
	}

	bool UniversallyUniqueID::operator<(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) < 0;
	}
	bool UniversallyUniqueID::operator<=(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) <= 0;
	}
	bool UniversallyUniqueID::operator>(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) > 0;
	}
	bool UniversallyUniqueID::operator>=(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) >= 0;
	}
}
