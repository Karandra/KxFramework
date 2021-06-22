#include "KxfPCH.h"
#include "String.h"

namespace kxf::Utility
{
	char* StringBuffer::PrepareNarrowChars()
	{
		m_NarrowChars.resize(m_Length);
		return m_NarrowChars.data();
	}
	wchar_t* StringBuffer::PrepareWideChars()
	{
		m_WideChars.resize(m_Length);
		return m_WideChars.data();
	}
	void StringBuffer::Finalize()
	{
		switch (m_Type)
		{
			case Type::NarrowChars:
			{
				m_Value = String::FromUTF8(m_NarrowChars);
				break;
			}
			case Type::WideChars:
			{
				m_Value = std::move(m_WideChars);
				break;
			}
		};
	}
}

namespace kxf::Utility
{
	std::optional<bool> ParseBool(const String& value)
	{
		if (value == kxS("false") || value == kxS("FALSE"))
		{
			return false;
		}
		else if (value == kxS("true") || value == kxS("TRUE"))
		{
			return true;
		}
		else if (auto iValue = value.ToInteger<int>())
		{
			return *iValue != 0;
		}
		else if (auto fValue = value.ToFloatingPoint())
		{
			return *fValue != 0;
		}
		return {};
	}
}
