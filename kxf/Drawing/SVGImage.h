#pragma once
#include "Common.h"
#include "IVectorImage.h"

namespace lunasvg
{
	class SVGDocument;
}

namespace kxf
{
	class KX_API SVGImage final: public RTTI::ExtendInterface<SVGImage, IVectorImage>
	{
		KxRTTI_DeclareIID_Using(SVGImage, ImageFormat::SVG.ToNativeUUID());

		private:
			std::shared_ptr<lunasvg::SVGDocument> m_Document;
			int m_DPI = -1;

		private:
			void AllocExclusive();

		public:
			SVGImage() = default;
			SVGImage(const SVGImage& other)
			{
				*this = other;
			}
			SVGImage(SVGImage&& other)
			{
				*this = std::move(other);
			}
			~SVGImage() = default;

		public:
			// IImage2D
			bool IsNull() const
			{
				return m_Document == nullptr;
			}
			bool IsSameAs(const IImage2D& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto svg = other.QueryInterface<SVGImage>())
				{
					return m_Document == svg->m_Document;
				}
				return false;
			}
			std::unique_ptr<IImage2D> CloneImage2D() const override;

			// IImage2D: Create, save and load
			void Create(const Size& size);
			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			// IImage2D: Properties
			Size GetSize() const override;
			ColorDepth GetColorDepth() const override
			{
				return ColorDepthDB::BPP32;
			}
			UniversallyUniqueID GetFormat() const override
			{
				return ImageFormat::SVG;
			}

			// IImage2D: Options
			std::optional<String> GetOption(const String& name) const override
			{
				return {};
			}
			std::optional<int> GetOptionInt(const String& name) const override;

			void SetOption(const String& name, int value) override;
			void SetOption(const String& name, const String& value) override
			{
			}

			// IImage2D: Conversion
			BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;
			GDIBitmap ToGDIBitmap(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;

		public:
			// IVectorImage
			Rect GetBoundingBox() const override;

		public:
			SVGImage& operator=(const SVGImage& other);
			SVGImage& operator=(SVGImage&& other) noexcept;
	};
}
