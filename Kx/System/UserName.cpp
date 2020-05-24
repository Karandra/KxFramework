#include "stdafx.h"
#include "UserName.h"
#include "Kx/General/UniversallyUniqueID.h"

namespace
{
	constexpr wxChar g_SAMSeparator = wxS('\\');
	constexpr wxChar g_UPNSeparator = wxS('@');
}

namespace kxf
{
	void UserName::AssignName(String name)
	{
		if (name.Contains(g_SAMSeparator))
		{
			name.BeforeFirst(g_SAMSeparator, &m_Domain);
			name.AfterFirst(g_SAMSeparator, &m_Name);
			m_NameFormat = UserNameFormat::DownLevel;
		}
		else if (name.Contains(g_UPNSeparator))
		{
			name.BeforeFirst(g_SAMSeparator, &m_Name);
			name.AfterFirst(g_SAMSeparator, &m_Domain);
			m_NameFormat = UserNameFormat::UserPrincipal;
		}
		else if (UniversallyUniqueID uuid(name); !uuid.IsNull())
		{
			m_Name = uuid.ToString(UUIDFormat::CurlyBraces);
			m_NameFormat = UserNameFormat::UniqueID;
		}
		else
		{
			m_Name = std::move(name);
			m_NameFormat = UserNameFormat::DownLevel;
		}
	}
	void UserName::AssignDomain(String domain)
	{
		if (!domain.Contains(g_SAMSeparator) && !domain.Contains(g_UPNSeparator))
		{
			m_Domain = std::move(domain);
		}
		else
		{
			m_Domain.clear();
		}
	}

	bool UserName::IsSameAs(const UserName& other) const noexcept
	{
		const bool namesSame = m_NameFormat == other.m_NameFormat && m_Name.IsSameAs(other.m_Name, StringOpFlag::IgnoreCase);
		if (!m_Domain.IsEmpty())
		{
			return namesSame && m_Domain.IsSameAs(other.m_Domain, StringOpFlag::IgnoreCase);
		}
		return namesSame;
	}
	String UserName::GetFullName(UserNameFormat withFormat) const
	{
		if (m_Domain.IsEmpty())
		{
			return m_Name;
		}
		else
		{
			switch (withFormat != UserNameFormat::None ? withFormat : m_NameFormat)
			{
				case UserNameFormat::Display:
				case UserNameFormat::UniqueID:
				{
					return m_Name;
				}
				case UserNameFormat::DownLevel:
				{
					return m_Domain + g_SAMSeparator + m_Name;
				}
				case UserNameFormat::UserPrincipal:
				{
					return m_Name + g_UPNSeparator + m_Domain;
				}
			};
		}
		return {};
	}
}
