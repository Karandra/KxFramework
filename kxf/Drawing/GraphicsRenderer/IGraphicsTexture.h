#pragma once
#include "Common.h"
#include "IGraphicsObject.h"

namespace kxf
{
	class Image;

	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KX_API IGraphicsTexture: public RTTI::ExtendInterface<IGraphicsTexture, IGraphicsObject>
	{
		KxRTTI_DeclareIID(IGraphicsTexture, {0xa7852135, 0x2bf0, 0x4737, {0xa4, 0xfb, 0xc9, 0x99, 0xc7, 0xa2, 0x57, 0xd3}});

		public:
			virtual ~IGraphicsTexture() = default;

		public:
			virtual SizeF GetPPI() const = 0;
			virtual SizeF GetSize() const = 0;
			virtual float GetWidth() const = 0;
			virtual float GetHeight() const = 0;
			virtual ColorDepth GetColorDepth() const = 0;

			virtual bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) = 0;
			virtual bool Save(IOutputStream& stream, ImageFormat format) const = 0;

			virtual std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const = 0;
			virtual void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) = 0;

			virtual Image ToImage() const = 0;
			virtual bool FromImage(const Image& image) = 0;
	};
}
