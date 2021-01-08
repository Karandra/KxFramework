#pragma once
#include "Common.h"
#include "GDIBitmap.h"
#include "GDIGraphicsRenderer.h"
#include "IGDIObject.h"
#include "../SVGImage.h"
#include "../BitmapImage.h"
#include "../GraphicsRenderer/IGraphicsTexture.h"

namespace kxf
{
	class KX_API GDIGraphicsTexture: public RTTI::ExtendInterface<GDIGraphicsTexture, IGraphicsTexture, IGDIObject>
	{
		KxRTTI_DeclareIID(GDIGraphicsTexture, {0xc046e1f4, 0xcff5, 0x419a, {0x98, 0xf5, 0xf4, 0x75, 0x6d, 0xdb, 0xc4, 0x8d}});

		protected:
			GDIGraphicsRenderer* m_Renderer = nullptr;
			GDIBitmap m_Bitmap;

		private:
			bool DoIsSameAs(const IObject& other) const
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsTexture>())
				{
					return m_Bitmap.IsSameAs(static_cast<const IGDIObject&>(object->m_Bitmap));
				}
				return false;
			}

		public:
			GDIGraphicsTexture() noexcept = default;
			GDIGraphicsTexture(GDIGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			GDIGraphicsTexture(GDIGraphicsRenderer& rendrer, const GDIBitmap& bitmap)
				:m_Renderer(&rendrer), m_Bitmap(bitmap)
			{
			}
			GDIGraphicsTexture(GDIGraphicsRenderer& rendrer, const BitmapImage& image)
				:m_Renderer(&rendrer), m_Bitmap(image.ToGDIBitmap())
			{
			}
			GDIGraphicsTexture(GDIGraphicsRenderer& rendrer, const SizeF& size, const Color& color)
				:m_Renderer(&rendrer), m_Bitmap(size, ColorDepthDB::BPP32)
			{
				wxMemoryDC dc(m_Bitmap.ToWxBitmap());
				dc.SetBackground(color);
				dc.Clear();
			}

		public:
			// IGDIObject
			bool IsSameAs(const IGDIObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIGraphicsTexture>(*this);
			}

			void* GetHandle() const override
			{
				return m_Bitmap.GetHandle();
			}
			void* DetachHandle() override
			{
				return m_Bitmap.DetachHandle();
			}
			void AttachHandle(void* handle) override
			{
				m_Bitmap.AttachHandle(handle);
			}

			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Bitmap.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsTexture>(*this);
			}

			GDIGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Bitmap.GetHandle();
			}

			// IGraphicsTexture
			SizeF GetDPI() const override
			{
				return SizeF::UnspecifiedSize();
			}
			SizeF GetSize() const override
			{
				return m_Bitmap.GetSize();
			}
			float GetWidth() const override
			{
				return m_Bitmap.GetWidth();
			}
			float GetHeight() const override
			{
				return m_Bitmap.GetHeight();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Bitmap.GetColorDepth();
			}

			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos) override
			{
				return m_Bitmap.Load(stream, format);
			}
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const override
			{
				return m_Bitmap.Save(stream, format);
			}

			std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const override
			{
				if (m_Bitmap)
				{
					return std::make_shared<GDIGraphicsTexture>(*m_Renderer, m_Bitmap.GetSubBitmap(rect));
				}
				return nullptr;
			}
			void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) override
			{
				m_Bitmap = m_Bitmap.ToGDIBitmap(size, interpolationQuality);
			}

			BitmapImage ToBitmapImage(const SizeF& size = SizeF::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::None) const override
			{
				return m_Bitmap.ToBitmapImage(size, interpolationQuality);
			}
			bool FromBitmapImage(const BitmapImage& image)
			{
				if (image)
				{
					m_Bitmap = image.ToGDIBitmap();
					return !m_Bitmap.IsNull();
				}
				return false;
			}

			// GDIGraphicsTexture
			const GDIBitmap& Get() const
			{
				return m_Bitmap;
			}
			GDIBitmap& Get()
			{
				return m_Bitmap;
			}
	};

	class KX_API GDIGraphicsVectorTexture: public RTTI::ExtendInterface<GDIGraphicsVectorTexture, IGraphicsTexture>
	{
		KxRTTI_DeclareIID(GDIGraphicsVectorTexture, {0xdffe01f1, 0xcc55, 0x4a8f, {0x84, 0x7d, 0xd0, 0xdd, 0xc5, 0x66, 0x4a, 0xed}});

		protected:
			GDIGraphicsRenderer* m_Renderer = nullptr;

			GDIBitmap m_Bitmap;
			SVGImage m_VectorImage;

		private:
			bool DoIsSameAs(const IObject& other) const
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsVectorTexture>())
				{
					return m_VectorImage.IsSameAs(object->m_VectorImage);
				}
				return false;
			}

			void Initialize(const SizeF& size)
			{
				if (!m_Bitmap || SizeF(m_Bitmap.GetSize()) != size)
				{
					m_Bitmap = m_VectorImage.ToGDIBitmap(size);
				}
			}
			void Invalidate()
			{
				m_Bitmap = {};
			}

		public:
			GDIGraphicsVectorTexture() noexcept = default;
			GDIGraphicsVectorTexture(GDIGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			GDIGraphicsVectorTexture(GDIGraphicsRenderer& rendrer, SVGImage vectorImage)
				:m_Renderer(&rendrer), m_VectorImage(std::move(vectorImage))
			{
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_VectorImage.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsVectorTexture>())
				{
					return m_VectorImage.IsSameAs(object->m_VectorImage);
				}
				return false;
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsVectorTexture>(*this);
			}

			GDIGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsTexture
			SizeF GetDPI() const override
			{
				auto dpi = m_VectorImage.GetOptionInt(ImageOption::DPI);
				return dpi ? SizeF(*dpi, *dpi) : SizeF::UnspecifiedSize();
			}
			SizeF GetSize() const override
			{
				return m_VectorImage.GetSize();
			}
			float GetWidth() const override
			{
				return m_VectorImage.GetWidth();
			}
			float GetHeight() const override
			{
				return m_VectorImage.GetHeight();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_VectorImage.GetColorDepth();
			}

			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos) override
			{
				Invalidate();
				return m_VectorImage.Load(stream, format);
			}
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const override
			{
				return m_VectorImage.Save(stream, format);
			}

			std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const override
			{
				return nullptr;
			}
			void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) override
			{
			}

			BitmapImage ToBitmapImage(const SizeF& size = SizeF::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::None) const override
			{
				return m_VectorImage.ToBitmapImage(size, interpolationQuality);
			}
			bool FromBitmapImage(const BitmapImage& image)
			{
				m_VectorImage = {};
				Invalidate();

				return false;
			}

			// GDIGraphicsTexture
			const GDIBitmap& Get(const SizeF& size) const
			{
				const_cast<GDIGraphicsVectorTexture&>(*this).Initialize(size);
				return m_Bitmap;
			}
			GDIBitmap& Get(const SizeF& size)
			{
				Initialize(size);
				return m_Bitmap;
			}
	};
}
