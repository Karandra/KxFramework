#pragma once
#include "Common.h"
#include "kxf/Utility/Numeric.h"
#include <algorithm>

namespace kxf
{
	class Angle final
	{
		public:
			constexpr static Angle FromNormalized(float value) noexcept
			{
				if (Utility::Abs(value) > 1.0f)
				{
					Utility::ModF(value, &value);
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

	#pragma warning(push, 0)
	#pragma warning(disable: 4005)

	#define Kx_AngleCmpOp(op, T1, T2, cmp)				inline constexpr bool operator op(T1 x, T2 y) noexcept { return cmp(x, y, op); }
	#define Kx_AngleCmp(left, right, op)				left.ToNormalized() op right.ToNormalized()
	wxFOR_ALL_COMPARISONS_3(Kx_AngleCmpOp, const Angle&, const Angle&, Kx_AngleCmp);

	#undef Kx_AngleCmp
	#undef Kx_AngleCmpOp
	#pragma warning(pop)
}
