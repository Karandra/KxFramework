#pragma once
#include "Common.h"
#include "Bitmap.h"
#include "IGDIObject.h"
#include "Private/Common.h"
#include <wx/region.h>

namespace kxf
{
	class KX_API Region: public RTTI::ExtendInterface<Region, IGDIObject>
	{
		KxRTTI_DeclareIID(Region, {0xd8f0f933, 0x9126, 0x4ef9, {0xa0, 0xf, 0x69, 0xe4, 0xd9, 0x9c, 0x7a, 0x97}});

		private:
			wxRegion m_Region;

		public:
			Region() = default;
			Region(const wxRegion& other)
				:m_Region(other)
			{
			}

			Region(const Rect& rect)
				:m_Region(rect)
			{
			}
			Region(const Point& pos, const Size& size)
				:m_Region(Rect(pos, size))
			{
			}
			Region(const Point& topLeft, const Point& bottomRight)
				:m_Region(topLeft, bottomRight)
			{
			}
			Region(const Bitmap& bitmap)
				:m_Region(bitmap.ToWxBitmap())
			{
			}
			Region(const Bitmap& bitmap, const Color& transparentColor, int tolerance = 0)
				:m_Region(bitmap.ToWxBitmap(), transparentColor, tolerance)
			{
			}

			Region(const Point* points, size_t count, PolygonFill fillMode = PolygonFill::OddEvenRule)
			{
				if (auto modeWx = Drawing::Private::MapPolygonFill(fillMode))
				{
					std::vector<wxPoint> pointsBuffer = {points, points + count};
					m_Region = wxRegion(pointsBuffer.size(), pointsBuffer.data(), *modeWx);
				}
			}

			template<size_t N>
			Region(const Point(&points)[N], PolygonFill fillMode = PolygonFill::OddEvenRule)
			{
				if (auto modeWx = Drawing::Private::MapPolygonFill(fillMode))
				{
					std::array<wxPoint, N> pointsBuffer;
					std::copy_n(std::begin(points), N, pointsBuffer.begin());

					m_Region = wxRegion(pointsBuffer.size(), pointsBuffer.data(), *modeWx);
				}
			}

			template<size_t N>
			Region(const std::array<Point, N>& points, PolygonFill fillMode = PolygonFill::OddEvenRule)
			{
				if (auto modeWx = Drawing::Private::MapPolygonFill(fillMode))
				{
					std::array<wxPoint, N> pointsBuffer;
					std::copy_n(std::begin(points), N, pointsBuffer.begin());

					m_Region = wxRegion(pointsBuffer.size(), pointsBuffer.data(), *modeWx);
				}
			}

			virtual ~Region() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Region.IsOk() || m_Region.IsEmpty();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other || m_Region.GetHRGN() == other.GetHandle())
				{
					return true;
				}
				else if (auto object = other.QueryInterface<Region>())
				{
					return m_Region == object->m_Region;
				}
				return false;
			}
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return std::make_unique<Region>(m_Region);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// Region
			const wxRegion& ToWxRegion() const noexcept
			{
				return m_Region;
			}
			wxRegion& ToWxRegion() noexcept
			{
				return m_Region;
			}

			void Clear()
			{
				m_Region.Clear();
			}
			Rect GetBox() const
			{
				return m_Region.GetBox();
			}
			bool Offset(const Point& pos)
			{
				return m_Region.Offset(pos);
			}
			Bitmap ToBitmap() const
			{
				return m_Region.ConvertToBitmap();
			}

			bool Contains(const Point& point) const
			{
				return m_Region.Contains(point);
			}
			bool Contains(const Rect& rect) const
			{
				return m_Region.Contains(rect);
			}

			bool Intersect(const Region& other)
			{
				return m_Region.Intersect(other.m_Region);
			}
			bool Intersect(const Rect& rect)
			{
				return m_Region.Intersect(rect);
			}

			bool Subtract(const Region& other)
			{
				return m_Region.Subtract(other.m_Region);
			}
			bool Subtract(const Rect& rect)
			{
				return m_Region.Subtract(rect);
			}

			bool Union(const Region& other)
			{
				return m_Region.Union(other.m_Region);
			}
			bool Union(const Rect& rect)
			{
				return m_Region.Union(rect);
			}
			bool Union(const Bitmap& bitmap)
			{
				return m_Region.Union(bitmap.ToWxBitmap());
			}
			bool Union(const Bitmap& bitmap, const Color& transparentColor, int tolerance = 0)
			{
				return m_Region.Union(bitmap.ToWxBitmap(), transparentColor, tolerance);
			}

			bool Xor(const Region& other)
			{
				return m_Region.Xor(other.m_Region);
			}
			bool Xor(const Rect& rect)
			{
				return m_Region.Xor(rect);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			Region& operator=(const Region& other)
			{
				m_Region = other.m_Region;

				return *this;
			}

			bool operator==(const Region& other) const
			{
				return m_Region == other.m_Region;
			}
			bool operator!=(const Region& other) const
			{
				return m_Region != other.m_Region;
			}
	};
}
