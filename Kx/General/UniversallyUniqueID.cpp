#include "stdafx.h"
#include "UniversallyUniqueID.h"
#include "Kx/System/ErrorCodeValue.h"

#include <Windows.h>
#include <rpcdce.h>
#include "Kx/System/UndefWindows.h"

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

	kxf::NativeUUID CreateFromString(const wchar_t* value) noexcept
	{
		NativeUUID uuid;
		if (::UuidFromStringW(reinterpret_cast<RPC_WSTR>(const_cast<wchar_t*>(value)), AsUUID(uuid)) == RPC_S_OK)
		{
			return uuid;
		}
		return {};
	}
	kxf::NativeUUID CreateFromString(const char* value) noexcept
	{
		NativeUUID uuid;
		if (::UuidFromStringA(reinterpret_cast<RPC_CSTR>(const_cast<char*>(value)), AsUUID(uuid)) == RPC_S_OK)
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
		Win32Error error(::UuidCreateSequential(AsGUID(uuid)));
		if (error.IsSuccess() || error.GetValue() == RPC_S_UUID_LOCAL_ONLY)
		{
			return uuid;
		}
		return {};
	}

	UniversallyUniqueID::UniversallyUniqueID(const char* value) noexcept
		:m_ID(CreateFromString(value))
	{
	}
	UniversallyUniqueID::UniversallyUniqueID(const wchar_t* value) noexcept
		:m_ID(CreateFromString(value))
	{
	}
	UniversallyUniqueID::UniversallyUniqueID(const String& value) noexcept
		:m_ID(CreateFromString(value.wx_str()))
	{
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
