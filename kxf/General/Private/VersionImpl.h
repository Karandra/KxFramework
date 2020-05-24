#pragma once
#include "../Common.h"
#include "kxf/General/String.h"
#include "kxf/General/DateTime.h"

namespace kxf
{
	class Version;
}

namespace kxf::Private::Version
{
	struct DefaultFormat final
	{
		public:
			static constexpr size_t ItemCount = 8;
			using Array = std::array<DefaultFormat, ItemCount>;

		public:
			wxChar m_String[ItemCount + 1] = {};
			int m_Numeric = -1;

		public:
			DefaultFormat() = default;
			DefaultFormat(int number, const wxChar* str = nullptr, size_t count = 0)
				:m_Numeric(number)
			{
				SetString(str, count);
			}

		public:
			bool HasNumeric() const
			{
				return m_Numeric >= 0;
			}
			bool HasString() const
			{
				return m_String[0] != wxS('\0');
			}
			void SetString(const wxChar* str, size_t count = 0)
			{
				if (!str)
				{
					str = wxS("");
					count = 0;
				}
				std::char_traits<wxChar>::copy(m_String, str, std::min(ItemCount, count));
			}
	};

	class StringAdapter final
	{
		private:
			std::basic_string_view<wxChar, std::char_traits<wxChar>> m_Str;

		public:
			StringAdapter(const wxChar* s)
				:m_Str(s)
			{
			}

		public:
			bool empty() const
			{
				return m_Str.empty();
			}

			bool operator==(const StringAdapter& other) const
			{
				return empty() == other.empty() || m_Str == other.m_Str;
			}
			bool operator!=(const StringAdapter& other) const
			{
				return !(*this == other);
			}
			bool operator<(const StringAdapter& other) const
			{
				return empty() || m_Str < other.m_Str;
			}
			bool operator<=(const StringAdapter& other) const
			{
				return empty() || m_Str <= other.m_Str;
			}
			bool operator>(const StringAdapter& other) const
			{
				return !empty() || m_Str > other.m_Str;
			}
			bool operator>=(const StringAdapter& other) const
			{
				return !empty() || m_Str >= other.m_Str;
			}
	};
}

namespace kxf::Private::Version
{
	enum class Cmp
	{
		LT = -1,
		EQ = 0,
		GT = 1,

		Invalid = -2
	};
	
	template<class T>
	static Cmp CompareValues(const T& left, const T& right)
	{
		if (left > right)
		{
			return Cmp::GT;
		}
		else if (left < right)
		{
			return Cmp::LT;
		}
		return Cmp::EQ;
	}

	Cmp Compare(const kxf::Version& left, const kxf::Version& right);

	bool Parse(const String& source, DefaultFormat::Array& items, size_t& componentCount);
	bool Parse(const String& source, DateTime& dateTime);

	String Format(const DefaultFormat::Array& items, size_t itemCount);
	String Format(const DateTime& dateTime);

	inline bool HasTimePart(const DateTime& dateTime)
	{
		return dateTime.GetHour() != 0 || dateTime.GetMinute() != 0 || dateTime.GetSecond() != 0 || dateTime.GetMillisecond() != 0;
	}
}
