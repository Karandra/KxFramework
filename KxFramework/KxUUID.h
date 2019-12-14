#pragma once
#include "KxFramework/KxFramework.h"
#include <rpcdce.h>

enum class KxUUIDStatus
{
	OK = 0,

	LocalOnly,
	NoAddress,
	Unknown,
};

class KX_API KxUUID
{
	public:
		KxUUID MakeUUID() noexcept
		{
			KxUUID uuid;
			uuid.Create();
			return uuid;
		}
		KxUUID MakeSequentialUUID() noexcept
		{
			KxUUID uuid;
			uuid.CreateSequential();
			return uuid;
		}

	private:
		::UUID m_ID = {0};

	public:
		KxUUID() noexcept;
		KxUUID(const ::UUID& other) noexcept
		{
			m_ID = other;
		}
		KxUUID(const KxUUID& other) noexcept
		{
			m_ID = other.m_ID;
		}
		KxUUID(KxUUID&& other) noexcept
		{
			*this = std::move(other);
		}

		KxUUID(const char* value) noexcept
		{
			FromString(value);
		}
		KxUUID(const wchar_t* value) noexcept
		{
			FromString(value);
		}
		KxUUID(const wxString& value) noexcept
		{
			FromString(value);
		}

	public:
		bool IsNull() const noexcept;
		KxUUIDStatus Create() noexcept;
		KxUUIDStatus CreateSequential() noexcept;
		
		size_t GetHash() const noexcept;

		::UUID ToNativeUUID() const noexcept
		{
			return m_ID;
		}
		wxString ToString() const;
		KxUUIDStatus FromString(const char* value) noexcept;
		KxUUIDStatus FromString(const wchar_t* value) noexcept;
		KxUUIDStatus FromString(const wxString& value) noexcept
		{
			return FromString(value.wx_str());
		}

	public:
		KxUUID& operator=(const ::UUID& other) noexcept
		{
			m_ID = other;
			return *this;
		}
		KxUUID& operator=(const KxUUID& other) noexcept
		{
			m_ID = other.m_ID;
			return *this;
		}
		KxUUID& operator=(KxUUID&& other) noexcept
		{
			m_ID = other.m_ID;
			other.m_ID = {0};
			return *this;
		}
		KxUUID& operator=(const wxString& value) noexcept
		{
			FromString(value);
			return *this;
		}

		operator ::UUID() const noexcept
		{
			return ToNativeUUID();
		}
		operator wxString() const noexcept
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
		bool operator==(const KxUUID& other) const noexcept;
		bool operator!=(const KxUUID& other) const noexcept
		{
			return !(*this == other);
		}
		bool operator<(const KxUUID& other) const noexcept;
		bool operator<=(const KxUUID& other) const noexcept;
		bool operator>(const KxUUID& other) const noexcept;
		bool operator>=(const KxUUID& other) const noexcept;

		bool operator==(const ::UUID& other) const noexcept;
		bool operator!=(const ::UUID& other) const noexcept
		{
			return !(*this == other);
		}
		bool operator<(const ::UUID& other) const noexcept;
		bool operator<=(const ::UUID& other) const noexcept;
		bool operator>(const ::UUID& other) const noexcept;
		bool operator>=(const ::UUID& other) const noexcept;
};

namespace std
{
	template<> struct hash<KxUUID>
	{
		size_t operator()(const KxUUID& uuid) const noexcept
		{
			return uuid.GetHash();
		}
	};
	template<> struct hash<::UUID>
	{
		size_t operator()(const ::UUID& uuid) const noexcept
		{
			return KxUUID(uuid).GetHash();
		}
	};
}
