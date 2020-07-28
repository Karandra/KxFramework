#include "stdafx.h"
#include "VersionImpl.h"
#include "../Version.h"
#include "kxf/General/StringFormater.h"

namespace
{
	template<kxf::VersionType type, class T>
	auto&& GetItem(T&& items)
	{
		using namespace kxf;

		return std::get<ToInt(type)>(items);
	}
}

namespace kxf::Private::Version
{
	Cmp Compare(const kxf::Version& left, const kxf::Version& right)
	{
		if (left.GetType() == right.GetType())
		{
			switch (left.GetType())
			{
				case VersionType::Default:
				{
					const auto& itemsLeft = GetItem<VersionType::Default>(left.m_Value);
					const auto& itemsRight = GetItem<VersionType::Default>(right.m_Value);

					for (size_t i = 0; i < std::max(left.GetComponentCount(), right.GetComponentCount()); i++)
					{
						auto left = itemsLeft[i].m_Numeric;
						auto right = itemsRight[i].m_Numeric;

						if (left < right)
						{
							return Cmp::LT;
						}
						else if (left > right)
						{
							return Cmp::GT;
						}
					}
					return Cmp::EQ;
				}
				case VersionType::DateTime:
				{
					return CompareValues(left.ToDateTime(), right.ToDateTime());
				}
			};
		}
		return Cmp::Invalid;
	}
	
	bool Parse(const String& source, DefaultFormat::Array& items, size_t& componentCount)
	{
		items.fill({});
		componentCount = 0;

		if (!source.IsEmpty())
		{
			// TODO: Rewrite this
			size_t pos = 0;
			while (pos != String::npos && componentCount < DefaultFormat::ItemCount)
			{
				wxChar* endPtr = nullptr;
				size_t endIndex = (endPtr - source.wc_str()) / sizeof(wxChar);

				items[componentCount].m_Numeric = std::wcstol(source.wc_str() + pos + (pos == 0 ? 0 : 1), &endPtr, 10);
				if (endPtr && endIndex != (size_t)pos && *endPtr != wxS('.'))
				{
					const wxChar* nextDotPtr = wcschr(endPtr, L'.');
					items[componentCount].SetString(endPtr, (nextDotPtr ? nextDotPtr : source.wc_str() + source.length()) - endPtr);
				}
				componentCount++;

				pos = source.Find(wxS('.'), pos + 1);
			}

			// Check
			for (size_t i = 0; i < componentCount; i++)
			{
				if (!items[i].HasNumeric())
				{
					componentCount = 0;
					return false;
				}
			}

			// Remove trailing zeros, but leave versions like '1.0' untouched.
			if (componentCount > 2)
			{
				for (size_t i = componentCount - 1; i > 1; i--)
				{
					if (items[i].m_Numeric == 0 && !items[i].HasString())
					{
						--componentCount;
					}
					else
					{
						break;
					}
				}
			}
			return true;
		}

		componentCount = 0;
		return false;
	}
	bool Parse(const String& source, DateTime& dateTime)
	{
		String::const_iterator it = source.begin();
		return dateTime.ParseISOCombined(source) || dateTime.ParseISOCombined(source, ' ') || dateTime.ParseISODate(source) || dateTime.ParseRFC822Date(source, &it);
	}

	String Format(const DefaultFormat::Array& items, size_t itemCount)
	{
		if (!items.empty() && itemCount != 0)
		{
			String result;
			result.reserve(itemCount * 2);

			for (size_t i = 0; i < itemCount; i++)
			{
				const int num = items[i].m_Numeric;
				const wxChar* string = items[i].m_String;

				// Don't print '0' for first element if it has non-empty string part
				if (i == 0 && num == 0 && *string)
				{
					result << string;
				}
				else
				{
					result << String::Format(wxS("%1"), num);
					if (*string)
					{
						result << string;
					}
				}

				// Add dot if this is not the last part
				if (i + 1 != itemCount)
				{
					result << wxS('.');
				}
			}
			return result;
		}
		return {};
	}
	String Format(const DateTime& dateTime)
	{
		if (dateTime.IsValid())
		{
			return HasTimePart(dateTime) ? dateTime.FormatISOCombined() : dateTime.FormatISODate();
		}
		return {};
	}
}
