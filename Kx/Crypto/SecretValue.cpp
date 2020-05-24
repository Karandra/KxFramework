#include "stdafx.h"
#include "SecretValue.h"

namespace kxf
{
	String SecretValue::ToString(const wxMBConv& conv) const
	{
		return wxString(m_Storage.data(), conv, m_Storage.size());
	}
	void SecretValue::Wipe() noexcept
	{
		if (!m_Storage.empty())
		{
			::RtlSecureZeroMemory(m_Storage.data(), m_Storage.size());
			m_Storage.clear();
		}
	}
}
