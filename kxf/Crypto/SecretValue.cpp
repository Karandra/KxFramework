#include "KxfPCH.h"
#include "SecretValue.h"
#include "kxf/Utility/Memory.h"

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
			Utility::SecureZeroMemory(m_Storage.data(), m_Storage.size());
			m_Storage.clear();
		}
	}
}
