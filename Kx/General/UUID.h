#pragma once
#include "Common.h"
#include "NativeUUID.h"

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
	class KX_API UUID final
	{
		public:
			static UUID Create() noexcept;
			static UUID CreateSequential() noexcept;

			static UUID CreateFromString(const wxString& value) noexcept;
			static UUID CreateFromString(const wchar_t* value) noexcept;
			static UUID CreateFromString(const char* value) noexcept;

		private:
			NativeUUID m_ID;

		public:
			UUID() noexcept = default;
			UUID(UUID&&) = default;
			UUID(const UUID&) = default;
			UUID(const NativeUUID& other) noexcept
			{
				m_ID = other;
			}
			UUID(const char* value) noexcept
			{
				*this = CreateFromString(value);
			}
			UUID(const wchar_t* value) noexcept
			{
				*this = CreateFromString(value);
			}
			UUID(const wxString& value) noexcept
			{
				*this = CreateFromString(value);
			}

		public:
			bool IsNull() const noexcept
			{
				return m_ID.IsNull();
			}
			size_t GetHash() const noexcept;

			NativeUUID ToNativeUUID() const noexcept
			{
				return m_ID;
			}
			wxString ToString(UUIDToStringFormat format = UUIDToStringFormat::None) const;

		public:
			UUID& operator=(UUID&&) = default;
			UUID& operator=(const UUID&) = default;
			UUID& operator=(const NativeUUID& other) noexcept
			{
				m_ID = other;
				return *this;
			}
			UUID& operator=(const wxString& value) noexcept
			{
				*this = CreateFromString(value);
				return *this;
			}

			operator NativeUUID() const noexcept
			{
				return ToNativeUUID();
			}
			operator wxString() const
			{
				return ToString();
			}

			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

		public:
			bool operator==(const UUID& other) const noexcept
			{
				return *this == other.m_ID;
			}
			bool operator!=(const UUID& other) const noexcept
			{
				return !(*this == other);
			}
			bool operator<(const UUID& other) const noexcept
			{
				return *this < other.m_ID;
			}
			bool operator<=(const UUID& other) const noexcept
			{
				return *this <= other.m_ID;
			}
			bool operator>(const UUID& other) const noexcept
			{
				return *this > other.m_ID;
			}
			bool operator>=(const UUID& other) const noexcept
			{
				return *this >= other.m_ID;
			}

			bool operator==(const NativeUUID& other) const noexcept
			{
				return m_ID == other;
			}
			bool operator!=(const NativeUUID& other) const noexcept
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
	struct hash<KxFramework::UUID>
	{
		size_t operator()(const KxFramework::UUID& uuid) const noexcept
		{
			return uuid.GetHash();
		}
	};
}
