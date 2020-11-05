#pragma once
#include "Common.h"
#include <wx/gdicmn.h>
#include <wx/geometry.h>
#include <cmath>
#include "kxf/General/Common.h"
#include "kxf/General/Angle.h"
#include <kxf/Utility/Common.h>
#include <kxf/Utility/Numeric.h>

namespace kxf::Geometry
{
	constexpr int DefaultCoord = wxDefaultCoord;

	enum class OutCode: uint32_t
	{
		Inside = 0,
		OutLeft = 1 << 0,
		OutRight = 1 << 1,
		OutTop = 1 << 2,
		OutBottom = 1 << 3
	};
}
namespace kxf
{
	KxFlagSet_Declare(Geometry::OutCode);

	class Size;
	class Point;
}

namespace kxf::Geometry
{
	template<class TDerived_, class TValue_>
	class BasicOrderedPair
	{
		public:
			using TDerived = TDerived_;
			using TValue = TValue_;

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
			constexpr BasicOrderedPair() noexcept = default;
			constexpr BasicOrderedPair(TValue x, TValue y) noexcept
				:m_X(x), m_Y(y)
			{
			}
			constexpr BasicOrderedPair(const BasicOrderedPair&) noexcept = default;
			constexpr BasicOrderedPair(BasicOrderedPair&&) noexcept = default;

		public:
			constexpr TDerived Clone() const noexcept
			{
				return Self();
			}
			constexpr bool IsFullySpecified() const noexcept
			{
				return m_X != DefaultCoord && m_Y != DefaultCoord;
			}
			constexpr TDerived& SetDefaults(const TDerived& other) noexcept
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

			constexpr TValue& X() noexcept
			{
				return m_X;
			}
			constexpr TValue& Y() noexcept
			{
				return m_Y;
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
			constexpr TDerived GetScaled(double x, double y) const noexcept
			{
				return Clone().Scale(x, y);
			}
			constexpr TDerived GetScaled(double factor) const noexcept
			{
				return Clone().Scale(factor);
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
			constexpr TDerived& IncBy(const TDerived& other) noexcept
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
			constexpr TDerived& DecBy(const TDerived& other) noexcept
			{
				return DecBy(other.m_X, other.m_Y);
			}

			constexpr TDerived& IncTo(const TDerived& other) noexcept
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
			constexpr TDerived& IncToIfSpecified(const TDerived& other) noexcept
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

			constexpr TDerived& DecTo(const TDerived& other) noexcept
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
			constexpr TDerived& DecToIfSpecified(const TDerived& other) noexcept
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

			constexpr double GetDistance(const TDerived& other) const noexcept
			{
				return std::sqrt(GetDistanceSquare(other));
			}
			constexpr double GetDistanceSquare(const TDerived& other) const noexcept
			{
				return (other.m_X - m_X) * (other.m_X - m_X) + (other.m_Y - m_Y) * (other.m_Y - m_Y);
			}

			constexpr double GetDotProduct(const TDerived& vector) const noexcept
			{
				return m_X * vector.m_X + m_Y * vector.m_Y;
			}
			constexpr double GetCrossProduct(const TDerived& vector) const noexcept
			{
				return m_X * vector.m_Y - vector.m_X * m_Y;
			}

		public:
			constexpr BasicOrderedPair& operator=(const BasicOrderedPair&) noexcept = default;
			constexpr BasicOrderedPair& operator=(BasicOrderedPair&&) noexcept = default;

			constexpr bool operator==(const TDerived& other) const noexcept
			{
				return m_X == other.m_X && m_Y == other.m_Y;
			}
			constexpr bool operator!=(const TDerived& other) const noexcept
			{
				return m_X != other.m_X || m_Y != other.m_Y;
			}

			constexpr TDerived& operator+=(const TDerived& other) noexcept
			{
				m_X += other.m_X;
				m_Y += other.m_Y;

				return Self();
			}
			constexpr TDerived& operator-=(const TDerived& other) noexcept
			{
				m_X -= other.m_X;
				m_Y -= other.m_Y;

				return Self();
			}
			constexpr TDerived& operator*=(TDerived n) noexcept
			{
				m_X *= n;
				m_Y *= n;

				return Self();
			}
			constexpr TDerived& operator/=(TDerived n) noexcept
			{
				m_X /= n;
				m_Y /= n;

				return Self();
			}
	};

	template<class TDerived, class TValue>
	constexpr TDerived operator-(const BasicOrderedPair<TDerived, TValue>& value) noexcept
	{
		return {-value.GetX(), -value.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator+(const BasicOrderedPair<TDerived, TValue>& left, const BasicOrderedPair<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() + right.GetX(), left.GetY() + right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator-(const BasicOrderedPair<TDerived, TValue>& left, const BasicOrderedPair<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() - right.GetX(), left.GetY() - right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator*(const BasicOrderedPair<TDerived, TValue>& left, const BasicOrderedPair<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() * right.GetX(), left.GetY() * right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator/(const BasicOrderedPair<TDerived, TValue>& left, const BasicOrderedPair<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() / right.GetX(), left.GetY() / right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator*(const BasicOrderedPair<TDerived, TValue>& left, TValue right) noexcept
	{
		return {left.GetX() * right, left.GetY() * right};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator/(const BasicOrderedPair<TDerived, TValue>& left, TValue right) noexcept
	{
		return {left.GetX() / right, left.GetY() / right};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator*(TValue left, const BasicOrderedPair<TDerived, TValue>& right) noexcept
	{
		return {left * right.GetX(), left * right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator/(TValue left, const BasicOrderedPair<TDerived, TValue>& right) noexcept
	{
		return {left / right.GetX(), left / right.GetY()};
	}
}

namespace kxf::Geometry
{
	template<class TDerived_, class TValue_, class TPoint_, class TSize_>
	class BasicRect
	{
		public:
			using TDerived = TDerived_;
			using TValue = TValue_;
			using TPoint = TPoint_;
			using TSize = TSize_;

		private:
			template<class T>
			using TSelf = BasicRect<T, TValue_, TPoint_, TSize_>;

		protected:
			TValue m_X = 0;
			TValue m_Y = 0;
			TValue m_Width = 0;
			TValue m_Height = 0;

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
			constexpr BasicRect() noexcept = default;
			constexpr BasicRect(TValue x, TValue y, TValue width, TValue height) noexcept
				:m_X(x), m_Y(y), m_Width(width), m_Height(height)
			{
			}
			constexpr BasicRect(const TPoint& topLeft, const TPoint& bottomRight) noexcept
				:m_X(topLeft.GetX()), m_Y(topLeft.GetY()), m_Width(bottomRight.GetX() - topLeft.GetX()), m_Height(bottomRight.GetY() - topLeft.GetY())
			{
				if (m_Width < 0)
				{
					m_Width = -m_Width;
					m_X = topLeft.GetX();
				}
				m_Width++;

				if (m_Height < 0)
				{
					m_Height = -m_Height;
					m_Y = topLeft.GetY();
				}
				m_Height++;
			}
			constexpr BasicRect(const TPoint& position, const TSize& size) noexcept
				:m_X(position.GetX()), m_Y(position.GetY()), m_Width(size.GetWidth()), m_Height(size.GetHeight())
			{
			}
			constexpr BasicRect(const TSize& size) noexcept
				:m_Width(size.GetWidth()), m_Height(size.GetHeight())
			{
			}
			constexpr BasicRect(const BasicRect&) noexcept = default;

		public:
			constexpr bool IsEmpty() const noexcept
			{
				return m_Width <= 0 || m_Height <= 0;
			}
			constexpr TDerived Clone() const noexcept
			{
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

			constexpr TValue GetWidth() const noexcept
			{
				return m_Width;
			}
			constexpr TDerived& SetWidth(TValue width) noexcept
			{
				m_Width = width;
				return Self();
			}

			constexpr TValue GetHeight() const noexcept
			{
				return m_Height;
			}
			constexpr TDerived& SetHeight(TValue height) noexcept
			{
				m_Height = height;
				return Self();
			}

			constexpr TValue& X() noexcept
			{
				return m_X;
			}
			constexpr TValue& Y() noexcept
			{
				return m_Y;
			}
			constexpr TValue& Width() noexcept
			{
				return m_Width;
			}
			constexpr TValue& Height() noexcept
			{
				return m_Height;
			}

		public:
			constexpr TPoint GetPosition() const noexcept
			{
				return {m_X, m_Y};
			}
			constexpr TDerived& SetPosition(const TPoint& value) noexcept
			{
				m_X = value.GetX();
				m_Y = value.GetY();
				return Self();
			}

			constexpr TSize GetSize() const noexcept
			{
				return {m_Width, m_Height};
			}
			constexpr TDerived& SetSize(const TSize& value) noexcept
			{
				m_Width = value.GetWidth();
				m_Height = value.GetHeight();
				return Self();
			}

			constexpr TValue GetLeft() const noexcept
			{
				return m_X;
			}
			constexpr TDerived& SetLeft(TValue left) noexcept
			{
				m_X = left;
				return Self();
			}
			constexpr TDerived& MoveLeftBy(TValue value) noexcept
			{
				m_X += value;
				return Self();
			}
			constexpr TDerived& MoveLeftTo(TValue value) noexcept
			{
				m_X = value;
				return Self();
			}

			constexpr TValue GetTop() const noexcept
			{
				return m_Y;
			}
			constexpr TDerived& SetTop(TValue top) noexcept
			{
				m_Y = top;
				return Self();
			}
			constexpr TDerived& MoveTopBy(TValue left) noexcept
			{
				m_Y += left;
				return Self();
			}
			constexpr TDerived& MoveTopTo(TValue value) noexcept
			{
				m_Y = value;
				return Self();
			}

			constexpr TValue GetRight() const noexcept
			{
				return m_X + m_Width - 1;
			}
			constexpr TDerived& SetRight(TValue right) noexcept
			{
				m_Width = right - m_X + 1;
				return Self();
			}
			constexpr TDerived& MoveRightBy(TValue value) noexcept
			{
				m_Width += value;
				return Self();
			}
			constexpr TDerived& MoveRightTo(TValue value) noexcept
			{
				m_X = value - m_Width;
				return Self();
			}

			constexpr TValue GetBottom() const noexcept
			{
				return m_Y + m_Height - 1;
			}
			constexpr TDerived& SetBottom(TValue bottom) noexcept
			{
				m_Height = bottom - m_Y + 1;
				return Self();
			}
			constexpr TDerived& MoveBottomBy(TValue value) noexcept
			{
				m_Height += value;
				return Self();
			}
			constexpr TDerived& MoveBottomTo(TValue value) noexcept
			{
				m_Y = value - m_Height;
				return Self();
			}

		public:
			constexpr TPoint GetTopLeft() const noexcept
			{
				return GetPosition();
			}
			constexpr TDerived& SetTopLeft(const TPoint& value) noexcept
			{
				return SetPosition(value);
			}
			constexpr TPoint GetLeftTop() const noexcept
			{
				return GetTopLeft();
			}
			constexpr TDerived& SetLeftTop(const TPoint& value) noexcept
			{
				return SetTopLeft(value);
			}

			constexpr TPoint GetBottomRight() const noexcept
			{
				return {GetRight(), GetBottom()};
			}
			constexpr TDerived& SetBottomRight(const TPoint& value) noexcept
			{
				SetRight(value.GetX());
				SetBottom(value.GetY());
				return Self();
			}
			constexpr TPoint GetRightBottom() const noexcept
			{
				return GetBottomRight();
			}
			constexpr TDerived& SetRightBottom(const TPoint& value) noexcept
			{
				return SetBottomRight(value);
			}

			constexpr TPoint GetTopRight() const noexcept
			{
				return {GetRight(), GetTop()};
			}
			constexpr TDerived& SetTopRight(const TPoint& value) noexcept
			{
				SetRight(value.GetX());
				SetTop(value.GetY());
				return Self();
			}
			constexpr TPoint GetRightTop() const noexcept
			{
				return GetTopRight();
			}
			constexpr TDerived& SetRightTop(const TPoint& value) noexcept
			{
				return SetTopRight(value);
			}

			constexpr TPoint GetBottomLeft() const noexcept
			{
				return {GetLeft(), GetBottom()};
			}
			constexpr TDerived& SetBottomLeft(const TPoint& value) noexcept
			{
				SetLeft(value.GetX());
				SetBottom(value.GetY());
				return Self();
			}
			constexpr TPoint GetLeftBottom() const noexcept
			{
				return GetBottomLeft();
			}
			constexpr TDerived& SetLeftBottom(const TPoint& value) noexcept
			{
				return SetBottomLeft(value);
			}

			constexpr bool Contains(TValue x, TValue y) const noexcept
			{
				return (x >= m_X) && (y >= m_Y) && (y - m_Y < m_Height) && (x - m_X < m_Width);
			}
			constexpr bool Contains(const TPoint& value) const noexcept
			{
				return Contains(value.GetX(), value.GetY());
			}
			constexpr bool Contains(const TDerived& other) noexcept
			{
				return Contains(other.GetTopLeft()) && Contains(other.GetBottomRight());
			}

			constexpr TDerived& Intersect(const TDerived& other) noexcept
			{
				TValue x2 = GetRight();
				TValue y2 = GetBottom();

				if (m_X < other.m_X)
				{
					m_X = other.m_X;
				}
				if (m_Y < other.m_Y)
				{
					m_Y = other.m_Y;
				}

				if (x2 > other.GetRight())
				{
					x2 = other.GetRight();
				}
				if (y2 > other.GetBottom())
				{
					y2 = other.GetBottom();
				}

				m_Width = x2 - m_X + 1;
				m_Height = y2 - m_Y + 1;
				if (m_Width <= 0 || m_Height <= 0)
				{
					m_Width = 0;
					m_Height = 0;
				}

				return Self();
			}
			constexpr TDerived GetIntersection(const TDerived& other) const noexcept
			{
				return Clone().Intersect(other);
			}
			constexpr bool Intersects(const TDerived& other) const noexcept
			{
				// If there is no intersection, both width and height are 0
				return GetIntersection(other).GetWidth() != 0;
			}

			constexpr TDerived& Union(const TDerived& other) noexcept
			{
				// Ignore empty rectangles: union with an empty rectangle shouldn't extend this one to (0, 0)
				if (m_Width == 0 || m_Height == 0)
				{
					*this = other;
				}
				else if (other.m_Width != 0 && other.m_Height != 0)
				{
					const TValue x1 = std::min(m_X, other.m_X);
					const TValue y1 = std::min(m_Y, other.m_Y);
					const TValue y2 = std::max(m_Y + m_Height, other.m_Height + other.m_Y);
					const TValue x2 = std::max(m_X + m_Width, other.m_Width + other.m_X);

					m_X = x1;
					m_Y = y1;
					m_Width = x2 - x1;
					m_Height = y2 - y1;
				}
				//else: we're not empty and 'other' is empty

				return Self();
			}
			constexpr TDerived GetUnion(const TDerived& other) const noexcept
			{
				return Clone().Union(other);
			}

			constexpr TDerived CenterIn(const TDerived& other, FlagSet<Orientation> direction = Orientation::Both) const noexcept
			{
				return
				{
					direction & Orientation::Horizontal ? other.m_X + (other.m_Width - m_Width) / 2 : m_X,
					direction & Orientation::Vertical ? other.m_Y + (other.m_Height - m_Height) / 2 : m_Y,
					m_Width,
					m_Height
				};
			}
			constexpr TDerived& Move(TValue dx, TValue dy) noexcept
			{
				m_X += dx;
				m_Y += dy;

				return Self();
			}
			constexpr TDerived& Move(const TPoint& offset) noexcept
			{
				return Move(offset.GetX(), offset.GetY());
			}

			constexpr TDerived& Inflate(TValue dx, TValue dy) noexcept
			{
				if (-2 * dx > m_Width)
				{
					// Don't allow deflate to eat more width than we have, a well-defined rectangle cannot have negative width.
					m_X += m_Width / 2;
					m_Width = 0;
				}
				else
				{
					// The inflate is valid.
					m_X -= dx;
					m_Width += 2 * dx;
				}

				if (-2 * dy > m_Height)
				{
					// Don't allow deflate to eat more height than we have, a well-defined rectangle cannot have negative height.
					m_Y += m_Height / 2;
					m_Height = 0;
				}
				else
				{
					// The inflate is valid.
					m_Y -= dy;
					m_Height += 2 * dy;
				}
				return Self();
			}
			constexpr TDerived& Inflate(TValue dxy) noexcept
			{
				return Inflate(dxy, dxy);
			}
			constexpr TDerived& Inflate(const TSize& dxy) noexcept
			{
				return Inflate(dxy.GetWidth(), dxy.GetHeight());
			}

			constexpr TDerived& Deflate(TValue dx, TValue dy) noexcept
			{
				return Inflate(-dx, -dy);
			}
			constexpr TDerived& Deflate(TValue dxy) noexcept
			{
				return Inflate(-dxy, -dxy);
			}
			constexpr TDerived& Deflate(const TSize& dxy) noexcept
			{
				return Inflate(-dxy.GetWidth(), -dxy.GetHeight());
			}

		public:
			constexpr bool EqualSize(const TDerived& other) noexcept
			{
				return GetSize() == other.GetSize();
			}
			constexpr void ConstrainTo(const TDerived& other) noexcept
			{
				if (GetLeft() < other.GetLeft())
				{
					SetLeft(other.GetLeft());
				}
				if (GetRight() > other.GetRight())
				{
					SetRight(other.GetRight());
				}
				if (GetBottom() > other.GetBottom())
				{
					SetBottom(other.GetBottom());
				}
				if (GetTop() < other.GetTop())
				{
					SetTop(other.GetTop());
				}
			}

			constexpr TPoint GetCenter() const noexcept
			{
				return {m_X + m_Width / 2, m_Y + m_Height / 2};
			}
			constexpr TDerived& MoveCenterTo(const TPoint& center) noexcept
			{
				m_X += center.m_X - (m_X + m_Width / 2);
				m_Y += center.m_Y - (m_Y + m_Height / 2);

				return Self();
			}
			constexpr FlagSet<OutCode> GetOutCode(const TPoint& point) const noexcept
			{
				FlagSet<OutCode> outCode;
				outCode.Add(OutCode::OutLeft, point.GetX() < m_X);
				outCode.Add(outCode, OutCode::OutRight, point.GetX() > m_X + m_Width);
				outCode.Add(outCode, OutCode::OutTop, point.GetY() < m_Y);
				outCode.Add(outCode, OutCode::OutBottom, point.GetY() > m_Y + m_Height);

				return outCode;
			}

			constexpr TDerived& Inset(const TPoint& point) noexcept
			{
				m_X += point.GetX();
				m_Y += point.GetY();
				m_Width -= 2 * point.GetX();
				m_Height -= 2 * point.GetY();

				return Self();
			}
			constexpr TDerived& Inset(const TDerived& other) noexcept
			{
				m_X += other.GetLeft();
				m_Y += other.GetTop();
				m_Width -= other.GetLeft() + other.GetRight();
				m_Height -= other.GetTop() + other.GetBottom();

				return Self();
			}
			
			constexpr TDerived& Scale(double dx, double dy, double dw, double dh) noexcept
			{
				m_X *= dx;
				m_Y *= dy;
				m_Width *= dw;
				m_Height *= dh;

				return Self();
			}
			constexpr TDerived& Scale(double dxy, double dwh) noexcept
			{
				return Scale(dxy, dxy, dwh, dwh);
			}
			constexpr TDerived& Scale(double scale) noexcept
			{
				return Scale(scale, scale);
			}
			constexpr TDerived& ScaleByRatio(double numerator, double denominator) noexcept
			{
				if (denominator != 0)
				{
					const double value = numerator / denominator;

					m_X *= value;
					m_Y *= value;
					m_Width *= value;
					m_Height *= value;
				}
				else
				{
					*this = {};
				}
				return Self();
			}
			constexpr TPoint Interpolate(double widthFactor, double heightFactor) const noexcept
			{
				return {m_X + m_Width * widthFactor, m_Y + m_Height * heightFactor};
			}

		public:
			constexpr bool operator==(const TDerived& other) const noexcept
			{
				return m_X == other.m_X && m_Y == other.m_Y && m_Width == other.m_Width && m_Height == other.m_Height;
			}
			constexpr bool operator!=(const TDerived& other) const noexcept
			{
				return m_X != other.m_X || m_Y != other.m_Y || m_Width != other.m_Width || m_Height != other.m_Height;
			}

			constexpr TDerived operator+(const TDerived& other) const noexcept
			{
				// Like Union() but don't treat empty rectangles specially
				const TValue x1 = std::min(m_X, other.m_X);
				const TValue y1 = std::min(m_Y, other.m_Y);
				const TValue y2 = std::max(m_Y + m_Height, other.m_Height + other.m_Y);
				const TValue x2 = std::max(m_X + m_Width, other.m_Width + other.m_X);

				return {x1, y1, x2 - x1, y2 - y1};
			}
			constexpr TDerived& operator+=(const TDerived& other) noexcept
			{
				Self() = Clone() + other;
				return Self();
			}

			constexpr TDerived operator*(const TDerived& other) const noexcept
			{
				// In original wxWidgets' code it's the same as 'operator+' for some reason
				return Clone() + other;
			}
			constexpr TDerived& operator*=(const TDerived& other) noexcept
			{
				Self() = Clone() * other;
				return Self();
			}
	};
}

namespace kxf
{
	class Point final: public Geometry::BasicOrderedPair<Point, int>
	{
		public:
			static constexpr Point UnspecifiedPosition() noexcept
			{
				return {Geometry::DefaultCoord, Geometry::DefaultCoord};
			}
			static constexpr Point FromSize(const Size& size) noexcept;

		public:
			using BasicOrderedPair::BasicOrderedPair;
			constexpr Point(const wxPoint& other) noexcept
				:BasicOrderedPair(other.x, other.y)
			{
			}
			constexpr Point(const wxRealPoint& other) noexcept
				:BasicOrderedPair(other.x, other.y)
			{
			}
			constexpr Point(const wxPoint2DInt& other) noexcept
				:BasicOrderedPair(other.m_y, other.m_y)
			{
			}
			constexpr Point(const wxPoint2DDouble& other) noexcept
				:BasicOrderedPair(other.m_y, other.m_y)
			{
			}

		public:
			using BasicOrderedPair::operator==;
			using BasicOrderedPair::operator!=;
			bool operator==(const wxPoint& other) const noexcept
			{
				return m_X == other.x && m_Y == other.y;
			}
			bool operator!=(const wxPoint& other) const noexcept
			{
				return m_X != other.x || m_Y != other.y;
			}

			operator wxPoint() const noexcept
			{
				return {m_X, m_Y};
			}
			operator wxRealPoint() const noexcept
			{
				return {static_cast<double>(m_X), static_cast<double>(m_Y)};
			}
			operator wxPoint2DInt() const noexcept
			{
				return {static_cast<wxInt32>(m_X), static_cast<wxInt32>(m_Y)};
			}
			operator wxPoint2DDouble() const noexcept
			{
				return {static_cast<wxDouble>(m_X), static_cast<wxDouble>(m_Y)};
			}
	};

	class Size final: public Geometry::BasicOrderedPair<Size, int>
	{
		public:
			static constexpr Size UnspecifiedSize() noexcept
			{
				return {Geometry::DefaultCoord, Geometry::DefaultCoord};
			}
			static constexpr Size FromPoint(const Point& point) noexcept;

		public:
			using BasicOrderedPair::BasicOrderedPair;
			constexpr Size(const wxSize& other) noexcept
				:BasicOrderedPair(other.GetWidth(), other.GetHeight())
			{
			}

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
			constexpr Size& SetHeight(TValue height) noexcept
			{
				return SetY(height);
			}
			
			constexpr TValue& Width() noexcept
			{
				return X();
			}
			constexpr TValue& Height() noexcept
			{
				return Y();
			}

		public:
			using BasicOrderedPair::operator==;
			using BasicOrderedPair::operator!=;
			bool operator==(const wxSize& other) const noexcept
			{
				return m_X == other.x && m_Y == other.y;
			}
			bool operator!=(const wxSize& other) const noexcept
			{
				return m_X != other.x || m_Y != other.y;
			}

			operator wxSize() const noexcept
			{
				return {static_cast<int>(m_X), static_cast<int>(m_Y)};
			}
	};

	class Rect final: public Geometry::BasicRect<Rect, int, Point, Size>
	{
		public:
			using BasicRect::BasicRect;
			constexpr Rect(const wxRect& other) noexcept
				:BasicRect(other.GetX(), other.GetY(), other.GetWidth(), other.GetHeight())
			{
			}
			constexpr Rect(const wxRect2DInt& other) noexcept
				:BasicRect(other.m_x, other.m_y, other.m_width, other.m_height)
			{
			}
			constexpr Rect(const wxRect2DDouble& other) noexcept
				:BasicRect(other.m_x, other.m_y, other.m_width, other.m_height)
			{
			}

		public:
			using BasicRect::operator==;
			using BasicRect::operator!=;
			bool operator==(const wxRect& other) const noexcept
			{
				return *this == Rect(other);
			}
			bool operator!=(const wxRect& other) const noexcept
			{
				return *this != Rect(other);
			}
			
			operator wxRect() const noexcept
			{
				return {static_cast<int>(m_X), static_cast<int>(m_Y), static_cast<int>(m_Width), static_cast<int>(m_Height)};
			}
	};
}
