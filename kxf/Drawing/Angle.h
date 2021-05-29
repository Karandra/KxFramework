#pragma once
#include "Common.h"
#include "kxf/Utility/Numeric.h"
#include <algorithm>

namespace kxf
{
	class Angle final
	{
		friend struct BinarySerializer<Angle>;

		public:
			constexpr static Angle FromNormalized(float value) noexcept
			{
				if (Utility::Abs(value) > 1.0f)
				{
					value = Utility::ModF(value);
				}

				Angle angle;
				angle.m_Value = value;
				return angle;
			}
			constexpr static Angle FromRadians(float radians) noexcept
			{
				return FromNormalized(radians / 2.0f);
			}
			constexpr static Angle FromDegrees(float degrees) noexcept
			{
				return FromNormalized(degrees / 360.0f);
			}

		private:
			float m_Value = -2.0f;

		public:
			constexpr Angle() noexcept = default;

		public:
			constexpr bool IsValid() const noexcept
			{
				return m_Value >= -1.0f && m_Value <= 1.0f;
			}

			constexpr float ToNormalized() const noexcept
			{
				return m_Value;
			}
			constexpr float ToRadians() const noexcept
			{
				return m_Value * 2.0f;
			}
			constexpr float ToDegrees() const noexcept
			{
				return m_Value * 360.0f;
			}

		public:
			constexpr explicit operator bool() const noexcept
			{
				return IsValid();
			}
			constexpr bool operator!() const noexcept
			{
				return !IsValid();
			}

			constexpr Angle& operator+=(const Angle& other) noexcept
			{
				*this = FromNormalized(m_Value + other.m_Value);
				return *this;
			}
			constexpr Angle& operator-=(const Angle& other) noexcept
			{
				*this = FromNormalized(m_Value - other.m_Value);
				return *this;
			}
			constexpr Angle& operator*=(float n) noexcept
			{
				*this = FromNormalized(m_Value * n);
				return *this;
			}
			constexpr Angle& operator/=(float n) noexcept
			{
				*this = FromNormalized(m_Value / n);
				return *this;
			}

			auto operator<=>(const Angle&) const noexcept = default;
	};
}

namespace kxf
{
	inline constexpr Angle operator+(const Angle& left, const Angle& right) noexcept
	{
		return Angle::FromNormalized(left.ToNormalized() + right.ToNormalized());
	}
	inline constexpr Angle operator-(const Angle& left, const Angle& right) noexcept
	{
		return Angle::FromNormalized(left.ToNormalized() - right.ToNormalized());
	}
}

namespace kxf
{
	template<>
	struct BinarySerializer<Angle> final
	{
		uint64_t Serialize(IOutputStream& stream, const Angle& value) const
		{
			return Serialization::WriteObject(stream, value.m_Value);
		}
		uint64_t Deserialize(IInputStream& stream, Angle& value) const
		{
			return Serialization::ReadObject(stream, value.m_Value);
		}
	};
}
