#pragma once
#include "Common.h"
#include <wx/gdicmn.h>
#include <wx/geometry.h>
#include <cmath>
#include "Kx/General/Angle.h"
#include <Kx/Utility/Numeric.h>

namespace KxFramework::Geometry
{
	template<class TDerived, class T>
	class BasicPoint2D
	{
		public:
			using TValue = T;

			static constexpr TValue DefaultCoord = wxDefaultCoord;

		protected:
			TValue m_X = DefaultCoord;
			TValue m_Y = DefaultCoord;

		protected:
			constexpr TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			constexpr const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		public:
			constexpr BasicPoint2D() noexcept = default;
			constexpr BasicPoint2D(TValue x, TValue y) noexcept
				:m_X(x), m_Y(y)
			{
			}
			constexpr BasicPoint2D(const wxSize& other) noexcept
				:m_X(other.GetX()), m_Y(other.GetY())
			{
			}
			constexpr BasicPoint2D(const wxPoint& other) noexcept
				:m_X(other.x), m_Y(other.y)
			{
			}
			constexpr BasicPoint2D(const wxRealPoint& other) noexcept
				:m_X(other.x), m_Y(other.y)
			{
			}
			constexpr BasicPoint2D(const wxPoint2DInt& other) noexcept
				:m_X(other.m_y), m_Y(other.m_y)
			{
			}
			constexpr BasicPoint2D(const wxPoint2DDouble& other) noexcept
				:m_X(other.m_y), m_Y(other.m_y)
			{
			}
			constexpr BasicPoint2D(const BasicPoint2D&) noexcept = default;

		public:
			constexpr TDerived Clone() const noexcept
			{
				return Self();
			}
			constexpr bool IsFullySpecified() const noexcept
			{
				return m_X != DefaultCoord && m_Y != DefaultCoord;
			}
			constexpr TDerived& SetDefaults(const BasicPoint2D& other) noexcept
			{
				if (m_X == DefaultCoord)
				{
					m_X = other.m_X;
				}
				if (m_Y == DefaultCoord)
				{
					m_X = other.m_X;
				}
				return Self();
			}

			constexpr TValue GetX() const noexcept
			{
				return m_X;
			}
			constexpr TDerived& SetX(TValue x) noexcept
			{
				m_X = x;
				return Self();
			}
			
			constexpr TValue GetY() const noexcept
			{
				return m_Y;
			}
			constexpr TDerived& SetY(TValue y) noexcept
			{
				m_Y = y;
				return Self();
			}

		public:
			constexpr TDerived& Scale(double x, double y) noexcept
			{
				m_X *= x;
				m_Y *= y;

				return Self();
			}
			constexpr TDerived& Scale(double factor) noexcept
			{
				return Scale(factor, factor);
			}

			constexpr TDerived& IncBy(TValue x, TValue y) noexcept
			{
				m_X += x;
				m_Y += y;

				return Self();
			}
			constexpr TDerived& IncBy(TValue value) noexcept
			{
				return IncBy(value, value);
			}
			constexpr TDerived& IncBy(const BasicPoint2D& other) noexcept
			{
				return IncBy(other.m_X, other.m_Y);
			}

			constexpr TDerived& DecBy(TValue x, TValue y) noexcept
			{
				return IncBy(-x, -y);
			}
			constexpr TDerived& DecBy(TValue value) noexcept
			{
				return DecBy(value, value);
			}
			constexpr TDerived& DecBy(const BasicPoint2D& other) noexcept
			{
				return DecBy(other.m_X, other.m_Y);
			}

			constexpr TDerived& IncTo(const BasicPoint2D& other) noexcept
			{
				if (other.m_X > m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y > m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}
			constexpr TDerived& IncToInSpecified(const BasicPoint2D& other) noexcept
			{
				if (other.m_X != DefaultCoord && other.m_X > m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y != DefaultCoord && other.m_Y > m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}

			constexpr TDerived& DecTo(const BasicPoint2D& other) noexcept
			{
				if (other.m_X < m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y < m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}
			constexpr TDerived& DecToInSpecified(const BasicPoint2D& other) noexcept
			{
				if (other.m_X != DefaultCoord && other.m_X < m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y != DefaultCoord && other.m_Y < m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}

		public:
			constexpr double GetVectorLength() const noexcept
			{
				return std::sqrt(static_cast<double>(m_X * m_X) + static_cast<double>(m_Y + m_Y));
			}
			constexpr TDerived& SetVectorLength(double length) noexcept
			{
				const double original = GetVectorLength();
				m_X = m_X * length / original;
				m_Y = m_Y * length / original;

				return Self();
			}
			constexpr TDerived& Normalize() const noexcept
			{
				return SetVectorLength(1.0);
			}

			constexpr Angle GetVectorAngle() const noexcept
			{
				if (Utility::AlmostZero(m_X))
				{
					return m_Y >= 0 ? Angle::FromDegrees(90) : Angle::FromDegrees(270);
				}
				else if (Utility::AlmostZero(m_Y))
				{
					return m_X >= 0 ? Angle::FromDegrees(0) : Angle::FromDegrees(180);
				}
				else
				{
					double deg = std::atan2(m_Y, m_X) * 180.0 / M_PI;
					if (deg < 0)
					{
						deg += 360;
					}
					return Angle::FromDegrees(deg);
				}
			}
			constexpr TDerived& SetVectorAngle(Angle angle) noexcept
			{
				const double length = GetVectorLength();
				const double degrees = angle.ToDegrees();

				m_X = length * std::cos(degrees / 180.0 * M_PI);
				m_Y = length * std::sin(degrees / 180.0 * M_PI);
				return Self();
			}

			constexpr double GetDistance(const BasicPoint2D& other) const noexcept
			{
				return std::sqrt(GetDistanceSquare(other));
			}
			constexpr double GetDistanceSquare(const BasicPoint2D& other) const noexcept
			{
				return (other.m_X - m_X) * (other.m_X - m_X) + (other.m_Y - m_Y) * (other.m_Y - m_Y);
			}

			constexpr double GetDotProduct(const BasicPoint2D& vector) const noexcept
			{
				return m_X * vector.m_X + m_Y * vector.m_Y;
			}
			constexpr double GetCrossProduct(const BasicPoint2D& vector) const noexcept
			{
				return m_X * vector.m_Y - vector.m_X * m_Y;
			}

		public:
			constexpr bool operator==(const BasicPoint2D& other) const noexcept
			{
				return m_X == other.m_X && m_Y == other.m_Y;
			}
			constexpr bool operator!=(const BasicPoint2D& other) const noexcept
			{
				return m_X != other.m_X || m_Y != other.m_Y;
			}

			constexpr TDerived& operator+=(const BasicPoint2D& other) noexcept
			{
				m_X += other.m_X;
				m_Y += other.m_Y;

				return Self();
			}
			constexpr TDerived& operator-=(const BasicPoint2D& other) noexcept
			{
				m_X -= other.m_X;
				m_Y -= other.m_Y;

				return Self();
			}
			constexpr TDerived& operator*=(TValue n) noexcept
			{
				m_X *= n;
				m_Y *= n;

				return Self();
			}
			constexpr TDerived& operator/=(TValue n) noexcept
			{
				m_X /= n;
				m_Y /= n;

				return Self();
			}

			constexpr TDerived operator-() noexcept
			{
				return {-m_X, -m_Y};
			}
			constexpr TDerived operator+(const BasicPoint2D& other) noexcept
			{
				return Clone() += other;
			}
			constexpr TDerived operator-(const BasicPoint2D& other) noexcept
			{
				return Clone() -= other;
			}
			constexpr TDerived operator*(TValue n) noexcept
			{
				return Clone() *= n;
			}
			constexpr TDerived operator/(TValue n) noexcept
			{
				return Clone() /= n;
			}
	};
}

namespace KxFramework
{
	class Point final: public Geometry::BasicPoint2D<Point, int>
	{
		public:
			using BasicPoint2D::BasicPoint2D;
	};

	class Size final: public Geometry::BasicPoint2D<Size, int>
	{
		public:
			using BasicPoint2D::BasicPoint2D;

		public:
			constexpr TValue GetWidth() const noexcept
			{
				return GetX();
			}
			constexpr Size& SetWidth(TValue width) noexcept
			{
				return SetX(width);
			}
			
			constexpr TValue GetHeight() const noexcept
			{
				return GetY();
			}
			constexpr Size& WetWidth(TValue height) noexcept
			{
				return SetY(height);
			}
	};
}
