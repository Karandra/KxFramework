#pragma once
#include "Common.h"
#include "IGraphicsObject.h"
#include "../GradientStops.h"

namespace kxf
{
	class IGraphicsTexture;
	class IGraphicsMatrix;
}

namespace kxf
{
	class KX_API IGraphicsBrush: public RTTI::ExtendInterface<IGraphicsBrush, IGraphicsObject>
	{
		KxRTTI_DeclareIID(IGraphicsBrush, {0x9e7e0f22, 0xc647, 0x4b60, {0x8f, 0x3a, 0x93, 0xd7, 0x2, 0x1, 0x6, 0xea}});

		public:
			virtual ~IGraphicsBrush() = default;

		public:
			virtual bool IsTransparent() const = 0;
	};

	class KX_API IGraphicsSolidBrush: public RTTI::ExtendInterface<IGraphicsSolidBrush, IGraphicsBrush>
	{
		KxRTTI_DeclareIID(IGraphicsSolidBrush, {0xc0252eaf, 0x8ce8, 0x482f, {0x9e, 0xe4, 0xd2, 0x40, 0x81, 0x95, 0xa7, 0xc9}});

		public:
			virtual Color GetColor() const = 0;
			virtual void SetColor(const Color& color) = 0;
	};

	class KX_API IGraphicsHatchBrush: public RTTI::ExtendInterface<IGraphicsHatchBrush, IGraphicsBrush>
	{
		KxRTTI_DeclareIID(IGraphicsHatchBrush, {0xa3dd188, 0x94ca, 0x40bf, {0x89, 0x2, 0x0, 0x47, 0x27, 0x84, 0xbc, 0x95}});

		public:
			virtual Color GetBackgroundColor() const = 0;
			virtual void SetBackgroundColor(const Color& color) = 0;

			virtual Color GetForegroundColor() const = 0;
			virtual void SetForegroundColor(const Color& color) = 0;

			virtual HatchStyle GetHatchStyle() const = 0;
			virtual void SetHatchStyle(HatchStyle style) = 0;
	};

	class KX_API IGraphicsTextureBrush: public RTTI::ExtendInterface<IGraphicsTextureBrush, IGraphicsBrush>
	{
		KxRTTI_DeclareIID(IGraphicsTextureBrush, {0xc5b52910, 0xa895, 0x4e02, {0xa2, 0x4d, 0xd7, 0xc7, 0xd2, 0xb7, 0xf5, 0xa7}});

		public:
			virtual std::shared_ptr<IGraphicsTexture> GetTexture() const = 0;
			virtual void SetTexture(std::shared_ptr<IGraphicsTexture> texture) = 0;

			virtual WrapMode GetWrapMode() const = 0;
			virtual void SetWrapMode(WrapMode wrapMode) = 0;
	};

	class KX_API IGraphicsLinearGradientBrush: public RTTI::ExtendInterface<IGraphicsLinearGradientBrush, IGraphicsBrush>
	{
		KxRTTI_DeclareIID(IGraphicsLinearGradientBrush, {0xdcb0c79a, 0x6bc1, 0x43a4, {0xad, 0x77, 0x41, 0xeb, 0x83, 0xe1, 0xdf, 0x69}});

		public:
			virtual RectF GetRect() const = 0;
			virtual void SetRect(const RectF& rect) = 0;

			virtual GradientStops GetLinearColors() const = 0;
			virtual void SetLinearColors(const GradientStops& colors) = 0;

			virtual AffineMatrixF GetTransform() const = 0;
			virtual void SetTransform(const AffineMatrixF& transform) = 0;

			virtual WrapMode GetWrapMode() const = 0;
			virtual void SetWrapMode(WrapMode wrapMode) = 0;
	};

	class KX_API IGraphicsRadialGradientBrush: public RTTI::ExtendInterface<IGraphicsRadialGradientBrush, IGraphicsBrush>
	{
		KxRTTI_DeclareIID(IGraphicsRadialGradientBrush, {0x69d4326b, 0x1948, 0x4751, {0x8a, 0xa5, 0x40, 0xd3, 0x1e, 0xbb, 0x19, 0xef}});

		public:
			virtual PointF GetCenterPoint() const = 0;
			virtual void SetCenterPoint(const PointF& point) = 0;

			virtual RectF GetRect() const = 0;
			virtual void SetRect(const RectF& rect) = 0;

			virtual GradientStops GetLinearColors() const = 0;
			virtual void SetLinearColors(const GradientStops& colors) = 0;

			virtual AffineMatrixF GetTransform() const = 0;
			virtual void SetTransform(const AffineMatrixF& transform) = 0;

			virtual WrapMode GetWrapMode() const = 0;
			virtual void SetWrapMode(WrapMode wrapMode) = 0;
	};
}
