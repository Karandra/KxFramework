#include "KxfPCH.h"
#include "String.h"
#include "kxf/Core/IEncodingConverter.h"

namespace kxf::Utility
{
	char* StringBuffer::PrepareNarrowChars()
	{
		m_NarrowChars.resize(m_Length);
		m_Type = Type::NarrowChars;

		return m_NarrowChars.data();
	}
	wchar_t* StringBuffer::PrepareWideChars()
	{
		m_WideChars.resize(m_Length);
		m_Type = Type::WideChars;

		return m_WideChars.data();
	}
	void StringBuffer::Finalize()
	{
		switch (m_Type)
		{
			case Type::NarrowChars:
			{
				if (m_NullTerminated)
				{
					m_Length = strnlen(m_NarrowChars.data(), m_NarrowChars.size());
					m_NarrowChars.resize(m_Length);
				}
				
				IEncodingConverter& converter = m_EncodingConverter ? *m_EncodingConverter : EncodingConverter_WhateverWorks;
				m_Value = converter.ToWideChar(m_NarrowChars);

				break;
			}
			case Type::WideChars:
			{
				if (m_NullTerminated)
				{
					m_Length = wcsnlen(m_WideChars.data(), m_WideChars.size());
					m_WideChars.resize(m_Length);
				}

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
