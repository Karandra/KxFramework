#pragma once
#include "Common.h"
#include "GDIBitmap.h"
#include "GDIGraphicsRenderer.h"
#include "IGDIObject.h"
#include "../Image.h"
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
					return m_Bitmap.IsSameAs(object->m_Bitmap);
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
			GDIGraphicsTexture(GDIGraphicsRenderer& rendrer, const Image& image)
				:m_Renderer(&rendrer), m_Bitmap(image.ToBitmap())
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
			SizeF GetPPI() const override
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

			bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) override
			{
				return m_Bitmap.Load(stream, format);
			}
			bool Save(IOutputStream& stream, ImageFormat format) const override
			{
				return m_Bitmap.Save(stream, format);
			}

			std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const override
			{
				return std::make_shared<GDIGraphicsTexture>(*m_Renderer, m_Bitmap.GetSubBitmap(rect));
			}
			void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) override
			{
				m_Bitmap = m_Bitmap.ToImage().RescaleThis(size, interpolationQuality).ToBitmap();
			}

			Image ToImage() const override
			{
				return m_Bitmap.ToImage();
			}
			bool FromImage(const Image& image)
			{
				if (image)
				{
					m_Bitmap = image.ToBitmap();
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

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}
	};
}
