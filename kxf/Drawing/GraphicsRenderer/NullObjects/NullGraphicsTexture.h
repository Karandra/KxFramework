#pragma once
#include "../Common.h"
#include "../IGraphicsTexture.h"
#include "../../Image.h"
#include "NullGraphicsRenderer.h"

namespace kxf::Drawing
{
	class KX_API NullGraphicsTexture final: public IGraphicsTexture
	{
		private:
			NullGraphicsRenderer m_Renderer;

		public:
			~NullGraphicsTexture() = default;

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return true;
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return other.IsNull();
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			IGraphicsRenderer& GetRenderer() override
			{
				return m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsTexture
			SizeF GetPPI() const override
			{
				return SizeF::UnspecifiedSize();
			}
			SizeF GetSize() const override
			{
				return SizeF::UnspecifiedSize();
			}
			float GetWidth() const override
			{
				return Geometry::DefaultCoord;
			}
			float GetHeight() const override
			{
				return Geometry::DefaultCoord;
			}
			ColorDepth GetColorDepth() const override
			{
				return {};
			}

			bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) override
			{
				return false;
			}
			bool Save(IOutputStream& stream, ImageFormat format) const override
			{
				return false;
			}

			std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const override
			{
				return nullptr;
			}
			void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) override
			{
			}

			Image ToImage() const override
			{
				return {};
			}
			bool FromImage(const Image& image) override
			{
				return false;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsTexture NullGraphicsTexture;
}
