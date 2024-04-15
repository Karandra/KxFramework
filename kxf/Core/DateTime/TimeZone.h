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

		private:
			wxDateTime::TimeZone ToWxOffset() const noexcept;
			void FromWxOffset(const wxDateTime::TimeZone& other) noexcept;
			void FromTimeZone(TimeZone tz) noexcept;

		public:
			TimeZoneOffset(const TimeSpan& offset = {}) noexcept
				:m_Offset(offset)
			{
			}
			TimeZoneOffset(const wxDateTime::TimeZone& other) noexcept
			{
				FromWxOffset(other);
			}
			TimeZoneOffset(TimeZone tz) noexcept
			{
				FromTimeZone(tz);
			}

		public:
			bool IsLocal() const noexcept;
			TimeSpan GetOffset() const noexcept;

		public:
			operator wxDateTime::TimeZone() const noexcept
			{
				return ToWxOffset();
			}

			auto operator<=>(const TimeZoneOffset&) const noexcept = default;
	};
}
