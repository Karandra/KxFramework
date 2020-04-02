#include "KxStdAfx.h"
#include "UUID.h"
#include <rpcdce.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework
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
}

namespace KxFramework
{
	UUID UUID::Create() noexcept
	{
		NativeUUID uuid;
		if (SUCCEEDED(::CoCreateGuid(AsGUID(uuid))))
		{
			return uuid;
		}
		return {};
	}
	UUID UUID::CreateSequential() noexcept
	{
		NativeUUID uuid;
		if (::UuidCreateSequential(AsGUID(uuid)) == RPC_S_OK)
		{
			return uuid;
		}
		return {};
	}

	UUID UUID::CreateFromString(const wxString& value) noexcept
	{
		return CreateFromString(value.wc_str());
	}
	UUID UUID::CreateFromString(const wchar_t* value) noexcept
	{
		NativeUUID uuid;
		if (::UuidFromStringW(reinterpret_cast<RPC_WSTR>(const_cast<wchar_t*>(value)), AsUUID(uuid)) == RPC_S_OK)
		{
			return uuid;
		}
		return {};
	}
	UUID UUID::CreateFromString(const char* value) noexcept
	{
		NativeUUID uuid;
		if (::UuidFromStringA(reinterpret_cast<RPC_CSTR>(const_cast<char*>(value)), AsUUID(uuid)) == RPC_S_OK)
		{
			return uuid;
		}
		return {};
	}

	size_t UUID::GetHash() const noexcept
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
	wxString UUID::ToString(UUIDToStringFormat format) const
	{
		wxString uuid = [&]() -> wxString
		{
			wchar_t* stringUUID = nullptr;
			if (::UuidToStringW(AsUUID(m_ID), reinterpret_cast<RPC_WSTR*>(&stringUUID)) == RPC_S_OK && stringUUID)
			{
				wxString temp = stringUUID;
				::RpcStringFreeW(reinterpret_cast<RPC_WSTR*>(&stringUUID));
				return temp;
			}
			return {};
		}();

		if (format & UUIDToStringFormat::UpperCase)
		{
			uuid.MakeUpper();
		}

		if (format & UUIDToStringFormat::URN)
		{
			uuid.Prepend(wxS("urn:uuid:"));
		}

		if (format & UUIDToStringFormat::CurlyBraces)
		{
			uuid.Prepend(wxS('{'));
			uuid.Append(wxS('}'));
		}
		else if (format & UUIDToStringFormat::Parentheses)
		{
			uuid.Prepend(wxS('('));
			uuid.Append(wxS(')'));
		}

		return uuid;
	}

	bool UUID::operator<(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) < 0;
	}
	bool UUID::operator<=(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) <= 0;
	}
	bool UUID::operator>(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) > 0;
	}
	bool UUID::operator>=(const NativeUUID& other) const noexcept
	{
		return Compare(m_ID, other) >= 0;
	}
}
