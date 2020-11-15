#pragma once
#include "Common.h"
#include "IGraphicsObject.h"

namespace kxf
{
	class KX_API IGraphicsMatrix: public RTTI::ExtendInterface<IGraphicsMatrix, IGraphicsObject>
	{
		KxRTTI_DeclareIID(IGraphicsMatrix, {0xa861a9d3, 0x77d5, 0x4cfc, {0xb5, 0x63, 0x3, 0xff, 0xd7, 0xf9, 0x7c, 0x52}});

		public:
			virtual ~IGraphicsMatrix() = default;

		public:
			virtual bool IsIdentity() const = 0;
			void SetIdentity()
			{
				SetElements(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
			}

			virtual void GetElements(float& m11, float& m12, float& m21, float& m22, float& tx, float& ty) const = 0;
			virtual void SetElements(float m11, float m12, float m21, float m22, float tx, float ty) = 0;

			virtual bool Invert() = 0;
			virtual void RotateTransform(Angle angle) = 0;
			virtual void Concat(const IGraphicsMatrix& other) = 0;

			virtual void ScaleTransform(float xScale, float yScale) = 0;
			virtual void ScaleTranslate(const Size& dxy) = 0;
			virtual SizeF TransformDistance(const SizeF& dxy) = 0;
			virtual PointF TransformPoint(const PointF& xy) = 0;
	};
}
