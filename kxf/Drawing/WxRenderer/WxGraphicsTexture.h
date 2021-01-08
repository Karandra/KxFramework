#pragma once
#include "Common.h"
#include "WxGraphicsRenderer.h"
#include "../BitmapImage.h"
#include "../SVGImage.h"
#include "../GraphicsRenderer/IGraphicsTexture.h"
#include <wx/graphics.h>

namespace kxf
{
	class KX_API WxGraphicsTexture: public RTTI::ExtendInterface<WxGraphicsTexture, IGraphicsTexture>
	{
		KxRTTI_DeclareIID(WxGraphicsTexture, {0x4c8a8b87, 0x9696, 0x4682, {0xb1, 0x8f, 0xc9, 0x55, 0x1f, 0xaa, 0xc, 0xd2}});

		protected:
			WxGraphicsRenderer* m_Renderer = nullptr;
			wxGraphicsBitmap m_Graphics;
			BitmapImage m_Image;

			bool m_Initialized = false;

		private:
			void Initialize()
			{
				if (!m_Initialized)
				{
					if (m_Image)
					{
						m_Image.InitAlpha();
						m_Graphics = m_Renderer->Get().CreateBitmapFromImage(m_Image.ToWxImage());
					}
					else
					{
						m_Graphics = {};
					}
					m_Initialized = true;
				}
			}
			void Invalidate()
			{
				m_Initialized = false;
			}

		public:
			WxGraphicsTexture() noexcept = default;
			WxGraphicsTexture(WxGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			WxGraphicsTexture(WxGraphicsRenderer& rendrer, const BitmapImage& image)
				:m_Renderer(&rendrer), m_Image(image.ToGDIBitmap())
			{
			}
			WxGraphicsTexture(WxGraphicsRenderer& rendrer, const SizeF& size, const Color& color)
				:m_Renderer(&rendrer), m_Image(size)
			{
				if (color)
				{
					m_Image.SetAreaRGBA(m_Image.GetSize(), color.GetFixed8());
				}
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Image.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<WxGraphicsTexture>())
				{
					return m_Image.IsSameAs(object->m_Image);
				}
				return false;
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<WxGraphicsTexture>(*this);
			}

			WxGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Graphics.GetGraphicsData();
			}

			// IGraphicsTexture
			SizeF GetDPI() const override
			{
				auto dpi = m_Image.GetOptionInt(ImageOption::DPI);
				return dpi ? SizeF(*dpi, *dpi) : SizeF::UnspecifiedSize();
			}
			SizeF GetSize() const override
			{
				return m_Image.GetSize();
			}
			float GetWidth() const override
			{
				return m_Image.GetWidth();
			}
			float GetHeight() const override
			{
				return m_Image.GetHeight();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Image.GetColorDepth();
			}

			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos) override
			{
				if (m_Image.Load(stream, format))
				{
					Invalidate();
					return true;
				}
				return false;
			}
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const override
			{
				return m_Image.Save(stream, format);
			}

			std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const override
			{
				if (m_Image)
				{
					return std::make_shared<WxGraphicsTexture>(*m_Renderer, m_Image.GetSubImage(rect));
				}
				return nullptr;
			}
			void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) override
			{
				m_Image.Rescale(size, interpolationQuality);
				Invalidate();
			}

			BitmapImage ToBitmapImage(const SizeF& size = SizeF::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override
			{
				return m_Image.ToBitmapImage(size, interpolationQuality);
			}
			bool FromBitmapImage(const BitmapImage& image) override
			{
				m_Image = image;
				Invalidate();

				return !m_Image.IsNull();
			}

			// WxGraphicsTexture
			const wxGraphicsBitmap& Get() const
			{
				const_cast<WxGraphicsTexture&>(*this).Initialize();
				return m_Graphics;
			}
			wxGraphicsBitmap& Get()
			{
				Initialize();
				return m_Graphics;
			}

			const BitmapImage& GetImage() const
			{
				return m_Image;
			}
			BitmapImage& GetImage()
			{
				return m_Image;
			}
	};

	class KX_API WxGraphicsVectorTexture: public RTTI::ExtendInterface<WxGraphicsVectorTexture, IGraphicsTexture>
	{
		KxRTTI_DeclareIID(WxGraphicsVectorTexture, {0xe067b8fd, 0x2be6, 0x48b4, {0x8d, 0x7c, 0xd1, 0x27, 0x85, 0xa8, 0x99, 0x25}});

		protected:
			WxGraphicsRenderer* m_Renderer = nullptr;
			wxGraphicsBitmap m_Graphics;

			SVGImage m_VectorImage;
			BitmapImage m_BitmapImage;
			bool m_Initialized = false;

		private:
			void Initialize(const SizeF& size)
			{
				if (m_VectorImage)
				{
					if (!m_Initialized || !m_BitmapImage || SizeF(m_BitmapImage.GetSize()) != size)
					{
						m_BitmapImage = m_VectorImage.ToBitmapImage(size);
						m_Graphics = m_Renderer->Get().CreateBitmapFromImage(m_BitmapImage.ToWxImage());

						m_Initialized = true;
					}
				}
				else
				{
					m_Graphics = {};
				}
			}
			void Invalidate()
			{
				m_Initialized = false;
				m_BitmapImage = {};
			}

		public:
			WxGraphicsVectorTexture() noexcept = default;
			WxGraphicsVectorTexture(WxGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			WxGraphicsVectorTexture(WxGraphicsRenderer& rendrer, SVGImage vectorImage)
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
				else if (auto object = other.QueryInterface<WxGraphicsVectorTexture>())
				{
					return m_VectorImage.IsSameAs(object->m_VectorImage);
				}
				return false;
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<WxGraphicsVectorTexture>(*this);
			}

			WxGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Graphics.GetGraphicsData();
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

			BitmapImage ToBitmapImage(const SizeF& size = SizeF::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override
			{
				return m_VectorImage.ToBitmapImage(size, interpolationQuality);
			}
			bool FromBitmapImage(const BitmapImage& image) override
			{
				m_VectorImage = {};
				Invalidate();

				return false;
			}

			// WxGraphicsTexture
			const wxGraphicsBitmap& Get(const SizeF& size) const
			{
				const_cast<WxGraphicsVectorTexture&>(*this).Initialize(size);
				return m_Graphics;
			}
			wxGraphicsBitmap& Get(const SizeF& size)
			{
				Initialize(size);
				return m_Graphics;
			}

			const SVGImage& GetImage() const
			{
				return m_VectorImage;
			}
			SVGImage& GetImage()
			{
				return m_VectorImage;
			}
	};
}
