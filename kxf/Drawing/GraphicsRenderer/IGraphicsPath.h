#pragma once
#include "Common.h"
#include "IGraphicsObject.h"

namespace kxf
{
	class IGraphicsMatrix;
}

namespace kxf
{
	class KX_API IGraphicsPath: public RTTI::ExtendInterface<IGraphicsPath, IGraphicsObject>
	{
		KxRTTI_DeclareIID(IGraphicsPath, {0x1c3cae6c, 0xf9a3, 0x4d24, {0xbd, 0xa0, 0x16, 0x18, 0x2d, 0x7d, 0x46, 0xc2}});

		public:
			virtual ~IGraphicsPath() = default;

		public:
			virtual bool ContainsPoint(const PointF& point, PolygonFill fill = PolygonFill::OddEvenRule) const = 0;
			virtual PointF GetCurrentPoint() const = 0;
			virtual PointF MoveToPoint(const PointF& point) = 0;
			virtual RectF GetBoundingBox() const = 0;

			virtual void AddArc(const PointF& point1, const PointF& point2, float radius) = 0;
			virtual void AddArc(const PointF& point, float radius, Angle start, Angle end, ClockDirection direction) = 0;
			virtual void AddCurve(const PointF& control1, const PointF& control12, const PointF& end) = 0;
			virtual void AddQuadCurve(const PointF& control, const PointF& end) = 0;
			virtual void AddLine(const PointF& point) = 0;
			virtual void AddCircle(const PointF& point1, float radius) = 0;
			virtual void AddEllipse(const RectF& rect) = 0;
			virtual void AddRect(const RectF& rect) = 0;
			virtual void AddRoundedRect(const RectF& rect, float radius) = 0;

			virtual void AddPath(std::shared_ptr<IGraphicsPath> path) = 0;
			virtual void CloseSubPath() = 0;

			virtual void Transform(AffineMatrixF path) = 0;
	};
}
