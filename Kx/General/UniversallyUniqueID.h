#pragma once
#include "Common.h"
#include "NativeUUID.h"
#include "String.h"

namespace KxFramework
{
	enum class UUIDToStringFormat
	{
		None = 0, // 123e4567-e89b-12d3-a456-426655440000
		URN = 1 << 1, // RFC 4122 format: urn:uuid:123e4567-e89b-12d3-a456-426655440000
		CurlyBraces = 1 << 2, // {123e4567-e89b-12d3-a456-426655440000}
		Parentheses = 1 << 3, // (123e4567-e89b-12d3-a456-426655440000)
		UpperCase = 1 << 4 // 123E4567-E89B-12D3-A456-426655440000
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(UUIDToStringFormat);
	}
}

namespace KxFramework
{
	class KX_API UniversallyUniqueID final
	{
		public:
			static UniversallyUniqueID Create() noexcept;
			static UniversallyUniqueID CreateSequential() noexcept;

		private:
			NativeUUID m_ID;

		public:
			constexpr UniversallyUniqueID() noexcept = default;
			constexpr UniversallyUniqueID(UniversallyUniqueID&&) noexcept = default;
			constexpr UniversallyUniqueID(const UniversallyUniqueID&) noexcept = default;
			constexpr UniversallyUniqueID(const NativeUUID& other) noexcept
				:m_ID(other)
			{
			}
			UniversallyUniqueID(const char* value) noexcept;
			UniversallyUniqueID(const wchar_t* value) noexcept;
			UniversallyUniqueID(const String& value) noexcept;

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_ID.IsNull();
			}
			constexpr size_t GetHash() const noexcept
			{
				size_t hash = m_ID.Data1;
				hash ^= m_ID.Data2;
				hash ^= m_ID.Data3;
				for (uint8_t d4: m_ID.Data4)
				{
					hash ^= d4;
				}
				return hash;
			}

			constexpr NativeUUID ToNativeUUID() const noexcept
			{
				return m_ID;
			}
			String ToString(UUIDToStringFormat format = UUIDToStringFormat::None) const;

		public:
			constexpr UniversallyUniqueID& operator=(UniversallyUniqueID&&) noexcept = default;
			constexpr UniversallyUniqueID& operator=(const UniversallyUniqueID&) noexcept = default;

			constexpr operator NativeUUID() const noexcept
			{
				return ToNativeUUID();
			}
			operator String() const
			{
				return ToString();
			}

			constexpr explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

		public:
			constexpr bool operator==(const UniversallyUniqueID& other) const noexcept
			{
				return *this == other.m_ID;
			}
			constexpr bool operator!=(const UniversallyUniqueID& other) const noexcept
			{
				return !(*this == other);
			}
			bool operator<(const UniversallyUniqueID& other) const noexcept
			{
				return *this < other.m_ID;
			}
			bool operator<=(const UniversallyUniqueID& other) const noexcept
			{
				return *this <= other.m_ID;
			}
			bool operator>(const UniversallyUniqueID& other) const noexcept
			{
				return *this > other.m_ID;
			}
			bool operator>=(const UniversallyUniqueID& other) const noexcept
			{
				return *this >= other.m_ID;
			}

			constexpr bool operator==(const NativeUUID& other) const noexcept
			{
				return m_ID == other;
			}
			constexpr bool operator!=(const NativeUUID& other) const noexcept
			{
				return !(*this == other);
			}
			bool operator<(const NativeUUID& other) const noexcept;
			bool operator<=(const NativeUUID& other) const noexcept;
			bool operator>(const NativeUUID& other) const noexcept;
			bool operator>=(const NativeUUID& other) const noexcept;
	};
}

namespace std
{
	template<>
	struct hash<KxFramework::NativeUUID>
	{
		constexpr size_t operator()(const KxFramework::NativeUUID& nativeUUID) const noexcept
		{
			return KxFramework::UniversallyUniqueID(nativeUUID).GetHash();
		}
	};

	template<>
	struct hash<KxFramework::UniversallyUniqueID>
	{
		constexpr size_t operator()(const KxFramework::UniversallyUniqueID& uuid) const noexcept
		{
			return uuid.GetHash();
		}
	};
}
