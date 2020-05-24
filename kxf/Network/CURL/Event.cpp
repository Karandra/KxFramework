#include "stdafx.h"
#include "Event.h"
#include "Session.h"

namespace
{
	using CharTraits = std::char_traits<char>;

	void NormalizeValue(kxf::String& value)
	{
		using namespace kxf;

		value.Trim();
		value.Trim(StringOpFlag::FromEnd);
		value.Replace(wxS("\r"), NullString);
		value.Replace(wxS("\n"), NullString);
	}
}

namespace kxf
{
	wxIMPLEMENT_DYNAMIC_CLASS(CURLEvent, FileOperationEvent);

	String CURLEvent::GetHeaderName() const
	{
		if (m_ResponseData)
		{
			if (const char* colon = CharTraits::find(m_ResponseData, m_ResponseLength, ':'))
			{
				String value = String::FromUTF8(m_ResponseData, colon - m_ResponseData);
				NormalizeValue(value);
				return value;
			}
		}
		return {};
	}
	String CURLEvent::GetHeaderValue() const
	{
		if (m_ResponseData)
		{
			if (const char* colon = CharTraits::find(m_ResponseData, m_ResponseLength, ':'))
			{
				// Skip colon itself and one space after it
				constexpr size_t offset = 2;

				String value = String::FromUTF8(colon + offset, m_ResponseLength - (colon - m_ResponseData) - offset);
				NormalizeValue(value);
				return value;
			}
		}
		return {};
	}
	String CURLEvent::GetHeaderLine() const
	{
		if (m_ResponseData)
		{
			String value = String::FromUTF8(m_ResponseData, m_ResponseLength);
			NormalizeValue(value);
			return value;
		}
		return {};
	}
}
