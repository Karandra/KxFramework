#include "KxStdAfx.h"
#include "KxFramework/KxVersion.h"

const KxVersion KxNullVersion = KxVersion();

bool KxVersionInternal::StringPartAdapter::operator==(const StringPartAdapter& other) const
{
	return empty() == other.empty() || m_Str == other.m_Str;
}
bool KxVersionInternal::StringPartAdapter::operator!=(const StringPartAdapter& other) const
{
	return !(*this == other);
}
bool KxVersionInternal::StringPartAdapter::operator<(const StringPartAdapter& other) const
{
	return empty() || m_Str < other.m_Str;
}
bool KxVersionInternal::StringPartAdapter::operator<=(const StringPartAdapter& other) const
{
	return empty() || m_Str <= other.m_Str;
}
bool KxVersionInternal::StringPartAdapter::operator>(const StringPartAdapter& other) const
{
	return !empty() || m_Str > other.m_Str;
}
bool KxVersionInternal::StringPartAdapter::operator>=(const StringPartAdapter& other) const
{
	return !empty() || m_Str >= other.m_Str;
}

//////////////////////////////////////////////////////////////////////////
KxVersion::Cmp KxVersion::Compare(const KxVersion& left, const KxVersion& right)
{
	if (left.GetType() == right.GetType())
	{
		switch (left.GetType())
		{
			case KxVERSION_DEFAULT:
			{
				for (size_t i = 0; i < std::max(left.m_Count, right.m_Count); i++)
				{
					auto n1 = left.GetNumericPart(i);
					auto n2 = right.GetNumericPart(i);

					if (n1 < n2)
					{
						return Cmp::LT;
					}
					else if (n1 > n2)
					{
						return Cmp::GT;
					}
				}
				return Cmp::EQ;
			}
			case KxVERSION_DATETIME:
			{
				return CompareValues(left.GetDateTime(), right.GetDateTime());
			}
			case KxVERSION_INTEGER:
			{
				return CompareValues(left.GetInteger(), right.GetInteger());
			}
		};
	}
	return Cmp::INV;
}

KxVersionType KxVersion::Create(const wxString& source, KxVersionType type)
{
	if (type == KxVERSION_INVALID)
	{
		// Order is important here
		if (ParseDateTime(source))
		{
			return KxVERSION_DATETIME;
		}
		if (ParseDefault(source))
		{
			return KxVERSION_DEFAULT;
		}
		if (ParseInteger(source))
		{
			return KxVERSION_INTEGER;
		}
	}
	else
	{
		bool isOK = false;
		switch (type)
		{
			case KxVERSION_DEFAULT:
			{
				isOK = ParseDefault(source);
				break;
			}
			case KxVERSION_DATETIME:
			{
				isOK = ParseDateTime(source);
				break;
			}
			case KxVERSION_INTEGER:
			{
				isOK = ParseInteger(source);
				break;
			}
		};
		
		if (isOK)
		{
			return type;
		}
	}
	return KxVERSION_INVALID;
}

/* Default */
bool KxVersion::ParseDefault(const wxString& source)
{
	if (!source.IsEmpty())
	{
		auto& version = SetDefault();

		size_t pos = 0;
		while (pos != wxString::npos && m_Count < KxVersionInternal::DefaultVersionSize)
		{
			wchar_t* endPtr = NULL;
			size_t endIndex = (endPtr - source.wc_str()) / sizeof(wchar_t);

			version[m_Count].m_Numeric = std::wcstol(source.wc_str() + pos + (pos == 0 ? 0 : 1), &endPtr, 10);
			if (endPtr && endIndex != (size_t)pos && *endPtr != L'.')
			{
				const wchar_t* nextDotPtr = wcschr(endPtr, L'.');
				version[m_Count].SetString(endPtr, (nextDotPtr ? nextDotPtr : source.wc_str() + source.Length()) - endPtr);
			}
			m_Count++;

			pos = source.find('.', pos + 1);
		}

		// Check
		if (m_Count != 0)
		{
			for (size_t i = 0; i < m_Count; i++)
			{
				if (!version[i].IsOk())
				{
					m_Count = 0;
					return false;
				}
			}
		}

		// Remove trailing zeros, but leave versions like '1.0' untouched.
		if (m_Count > 2)
		{
			for (size_t i = m_Count - 1; i > 1; i--)
			{
				if (GetNumericPart(i) == 0 && !GetStringPart(i))
				{
					--m_Count;
				}
				else
				{
					break;
				}
			}
		}
		return true;
	}

	m_Count = 0;
	return false;
}
wxString KxVersion::FormatDefault() const
{
	wxString out;
	for (size_t i = 0; i < m_Count; i++)
	{
		int num = GetNumericPart(i);
		const wchar_t* string = GetStringPart(i);

		// Don't print '0' for first element if it has non-empty string part
		if (i == 0 && num == 0 && string)
		{
			out << string;
		}
		else
		{
			out << num;
			if (string)
			{
				out << string;
			}
		}

		// Add dot if this is not the last part
		if (i + 1 != m_Count)
		{
			out << '.';
		}
	}
	return out;
}

/* DateTime */
bool KxVersion::HasTimePart() const
{
	const auto& v = GetDateTime();

	// Can omit milliseconds
	return v.GetHour() != 0 || v.GetMinute() != 0 || v.GetSecond() != 0;
}
bool KxVersion::ParseDateTime(const wxString& source)
{
	DateTimeT& t = SetDateTime();
	wxString::const_iterator it = source.begin();
	bool isOK = t.ParseISOCombined(source) || t.ParseISOCombined(source, ' ') || t.ParseISODate(source) || t.ParseRfc822Date(source, &it);

	t.ResetTime();
	return isOK;
}
wxString KxVersion::FormatDateTime() const
{
	return HasTimePart() ? GetDateTime().FormatISOCombined() : GetDateTime().FormatISODate();
}

/* Integer */
bool KxVersion::ParseInteger(const wxString& source)
{
	long long value = -1;
	if (source.ToLongLong(&value) && value >= 0)
	{
		SetInteger(value);
		return true;
	}
	return false;
}
wxString KxVersion::FormatInteger() const
{
	return wxString() << GetInteger();
}

//////////////////////////////////////////////////////////////////////////
KxVersion::KxVersion(const wxString& s, KxVersionType type)
{
	m_Type = Create(s, type);
}
KxVersion::KxVersion(const DateTimeT& t)
	:m_Type(t.IsValid() ? KxVERSION_DATETIME : KxVERSION_INVALID)
{
	SetDateTime(t).ResetTime();
}
KxVersion::KxVersion(const IntegerT& i)
	:m_Type(CheckInteger(i) ? KxVERSION_INTEGER : KxVERSION_INVALID)
{
	SetInteger(i);
}
KxVersion::KxVersion(const wxVersionInfo& versionInfo)
	:m_Type(KxVERSION_DEFAULT), m_Count(3)
{
	auto& version = SetDefault();
	version[0].m_Numeric = versionInfo.GetMajor();
	version[1].m_Numeric = versionInfo.GetMinor();
	version[2].m_Numeric = versionInfo.GetMicro();
}
KxVersion::~KxVersion()
{
}

int KxVersion::GetPartsCount() const
{
	switch (m_Type)
	{
		case KxVERSION_DEFAULT:
		{
			return m_Count;
		}
		case KxVERSION_DATETIME:
		{
			return HasTimePart() ? 2 : 1;
		}
		case KxVERSION_INTEGER:
		{
			return 1;
		}
	};
	return 0;
}
wxString KxVersion::ToString() const
{
	switch (m_Type)
	{
		case KxVERSION_DEFAULT:
		{
			return FormatDefault();
		}
		case KxVERSION_DATETIME:
		{
			return FormatDateTime();
		}
		case KxVERSION_INTEGER:
		{
			return FormatInteger();
		}
	};
	return wxEmptyString;
}
wxVersionInfo KxVersion::ToWxVersionInfo(const wxString& name, const wxString& description, const wxString& copyright) const
{
	auto GetPart = [this](size_t index)
	{
		int value = GetNumericPart(index);
		return value >= 0 ? value : 0;
	};
	return wxVersionInfo(name, GetPart(0), GetPart(1), GetPart(2), description, copyright);
}

bool KxVersion::operator==(const KxVersion& other) const
{
	return Compare(*this, other) == Cmp::EQ;
}
bool KxVersion::operator!=(const KxVersion& other) const
{
	return !(*this == other);
}
bool KxVersion::operator<(const KxVersion& other) const
{
	return Compare(*this, other) == Cmp::LT;
}
bool KxVersion::operator<=(const KxVersion& other) const
{
	return (*this < other) || (*this == other);
}
bool KxVersion::operator>(const KxVersion& other) const
{
	return Compare(*this, other) == Cmp::GT;
}
bool KxVersion::operator>=(const KxVersion& other) const
{
	return (*this > other) || (*this == other);
}
