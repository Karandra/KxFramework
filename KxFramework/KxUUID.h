/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

enum class KxUUIDStatus
{
	OK = 0,

	LocalOnly,
	NoAddress,
	Unknown,
};

class KX_API KxUUID
{
	private:
		::UUID m_ID = {0};

	public:
		KxUUID() noexcept;
		KxUUID(const UUID& uuid) noexcept
		{
			*this = uuid;
		}
		KxUUID(const KxUUID& uuid) noexcept
		{
			*this = uuid;
		}
		KxUUID(KxUUID&& uuid) noexcept
		{
			*this = std::move(uuid);
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

		const ::UUID& GetID() const
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

	public:
		bool operator==(const KxUUID& other) const noexcept;
		bool operator!=(const KxUUID& other) const noexcept
		{
			return !(*this == other);
		}

		bool operator==(const ::UUID& other) const noexcept;
		bool operator!=(const ::UUID& other) const noexcept
		{
			return !(*this == other);
		}
};
