#pragma once
#include "Common.h"
#include "TimeSpan.h"

namespace kxf
{
	using TimeZone = wxDateTime::TZ;
}

namespace kxf
{
	class KX_API TimeZoneOffset final
	{
		private:
			TimeSpan m_Offset;

		public:
			TimeZoneOffset(const TimeSpan& offset = {}) noexcept
				:m_Offset(offset)
			{
			}
			TimeZoneOffset(const wxDateTime::TimeZone& other) noexcept
			{
				*this = other;
			}
			TimeZoneOffset(TimeZone tz) noexcept
			{
				*this = tz;
			}

		public:
			bool IsLocal() const noexcept;
			TimeSpan GetOffset() const noexcept;

		public:
			TimeZoneOffset& operator=(TimeZone tz) noexcept;
			TimeZoneOffset& operator=(const wxDateTime::TimeZone& other) noexcept;
			operator wxDateTime::TimeZone() const noexcept;

			bool operator==(const TimeZoneOffset& other) const noexcept
			{
				return m_Offset == other.m_Offset;
			}
			bool operator!=(const TimeZoneOffset& other) const noexcept
			{
				return !(*this == other);
			}
	};
}
