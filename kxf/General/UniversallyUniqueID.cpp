#include "stdafx.h"
#include "UniversallyUniqueID.h"
#include "LocallyUniqueID.h"
#include "kxf/System/HResult.h"
#include "kxf/System/Win32Error.h"
#include "kxf/Utility/Common.h"

#include <Windows.h>
#include <rpcdce.h>
#include "kxf/System/UndefWindows.h"

namespace
{
	constexpr kxf::XChar g_DefaultSeparator[] = wxS("-");

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

	int Compare(const kxf::NativeUUID& left, const kxf::NativeUUID& right) noexcept
	{
		RPC_STATUS status = RPC_S_OK;
		return ::UuidCompare(const_cast<::UUID*>(AsUUID(left)), const_cast<::UUID*>(AsUUID(right)), &status);
	}

	kxf::NativeUUID DoCreateFromString(const wchar_t* value) noexcept
	{
		using namespace kxf;

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
		using namespace kxf;

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
		std::memcpy(&bytes[sizeof(low)], &high, sizeof(high));

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

	String UniversallyUniqueID::ToString(FlagSet<UUIDFormat> format, const String& separator) const
	{
		String uuid;
		bool shouldReplaceDefaultSeparator = false;

		// Decide on brace character
		std::pair<XChar, XChar> braces = {0, 0};
		if (format & UUIDFormat::CurlyBraces)
		{
			braces = {wxS('{'), wxS('}')};
		}
		else if (format & UUIDFormat::SquareBraces)
		{
			braces = {wxS('['), wxS(']')};
		}
		else if (format & UUIDFormat::AngleBraces)
		{
			braces = {wxS('<'), wxS('>')};
		}
		else if (format & UUIDFormat::Parentheses)
		{
			braces = {wxS('('), wxS(')')};
		}

		if (format & UUIDFormat::HexPrefix || format & UUIDFormat::Grouped)
		{
			auto DoPart = [&](auto value, size_t width, bool suppressSeparator = false)
			{
				String part = std::move(StringFormatter::Formatter(wxS("%1"))(value, width, 16, wxS('0'))).ToString();
				part.Truncate(width);

				if (format & UUIDFormat::HexPrefix)
				{
					uuid += wxS("0x");
				}
				uuid += std::move(part);

				if (!suppressSeparator)
				{
					uuid += separator.IsEmpty() ? g_DefaultSeparator : separator;
				}
			};

			if (format & UUIDFormat::Grouped)
			{
				DoPart(m_ID.Data1, 8);
				DoPart(m_ID.Data2, 4);
				DoPart(m_ID.Data3, 4);

				if (braces.first != 0)
				{
					uuid += braces.first;
				}
				for (size_t i = 0; i < std::size(m_ID.Data4); i++)
				{
					DoPart(m_ID.Data4[i], 2, braces.first != 0 && i + 1 == std::size(m_ID.Data4));
				}
				if (braces.second != 0)
				{
					uuid += braces.second;
				}
			}
			else
			{
				DoPart(m_ID.Data1, 8);
				DoPart(m_ID.Data2, 4);
				DoPart(m_ID.Data3, 4);
				DoPart(Utility::IntFromLowHigh<uint16_t>(m_ID.Data4[0], m_ID.Data4[1]), 4);

				// Combine the last 6 bytes into an 'uint64_t' and print it
				auto d4_23 = Utility::IntFromLowHigh<uint16_t>(m_ID.Data4[2], m_ID.Data4[3]);
				auto d4_45 = Utility::IntFromLowHigh<uint16_t>(m_ID.Data4[4], m_ID.Data4[5]);
				auto d4_67 = Utility::IntFromLowHigh<uint16_t>(m_ID.Data4[6], m_ID.Data4[7]);
				auto d4_x0 = Utility::IntFromLowHigh<uint32_t>(d4_23, d4_45);
				auto d4_x1 = Utility::IntFromLowHigh<uint32_t>(d4_67, static_cast<uint16_t>(0));
				DoPart(Utility::IntFromLowHigh<uint64_t>(d4_x0, d4_x1), 12, true);
			}
		}
		else
		{
			// Use standard to string conversion
			uuid = [&]() -> String
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
			shouldReplaceDefaultSeparator = true;
		}

		// Make uppercase
		if (format & UUIDFormat::UpperCase)
		{
			uuid.MakeUpper();
		}

		// Add braces or parentheses if required
		if (braces.first != 0 && braces.second != 0)
		{
			uuid.Prepend(braces.first);
			uuid.Append(braces.second);
		}

		// Append URN format prefix
		if (format & UUIDFormat::URN)
		{
			uuid.Prepend(wxS("urn:uuid:"));
		}

		// Replace default separator with a user supplied one if required
		if (shouldReplaceDefaultSeparator && !separator.IsEmpty() && separator != g_DefaultSeparator)
		{
			uuid.Replace(g_DefaultSeparator, separator);
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
