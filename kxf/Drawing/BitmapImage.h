#pragma once
#include "Common.h"
#include "IBitmapImage.h"
#include "kxf/Serialization/BinarySerializer.h"
#include <wx/image.h>

namespace kxf
{
	class GDICursor;
	class GDIBitmap;
	class GDIIcon;
}

namespace kxf
{
	class KX_API BitmapImage final: public RTTI::ExtendInterface<BitmapImage, IBitmapImage>
	{
		KxRTTI_DeclareIID(BitmapImage, {0x84c3ee9b, 0x62dc, 0x4d8c, {0x8c, 0x94, 0xd1, 0xbc, 0xaf, 0x68, 0xfb, 0xc1}});

		public:
			static size_t GetImageCount(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any);

		private:
			wxImage m_Image;

		public:
			BitmapImage() = default;
			BitmapImage(const wxImage& other)
				:m_Image(other)
			{
			}

			BitmapImage(const GDIIcon& other);
			BitmapImage(const GDIBitmap& other);
			BitmapImage(const GDICursor& other);
			BitmapImage(const BitmapImage& other)
				:m_Image(other.m_Image)
			{
			}

			BitmapImage(const Size& size)
				:m_Image(size.GetWidth(), size.GetHeight(), false)
			{
			}
			BitmapImage(const Size& size, uint8_t* rgb)
				:m_Image(size.GetWidth(), size.GetHeight(), rgb, true)
			{
			}
			BitmapImage(const Size& size, uint8_t* rgb, uint8_t* alpha)
				:m_Image(size.GetWidth(), size.GetHeight(), rgb, alpha, true)
			{
			}
			BitmapImage(const Size& size, wxMemoryBuffer& rgb)
				:m_Image(size.GetWidth(), size.GetHeight(), static_cast<unsigned char*>(rgb.release()), false)
			{
			}
			BitmapImage(const Size& size, wxMemoryBuffer& rgb, wxMemoryBuffer& alpha)
				:m_Image(size.GetWidth(), size.GetHeight(), static_cast<unsigned char*>(rgb.release()), static_cast<unsigned char*>(alpha.release()), false)
			{
			}

			virtual ~BitmapImage() = default;

		public:
			// IImage2D
			bool IsNull() const
			{
				return !m_Image.IsOk();
			}
			bool IsSameAs(const IImage2D& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto image = other.QueryInterface<BitmapImage>())
				{
					return m_Image.IsSameAs(image->m_Image);
				}
				return false;
			}
			std::unique_ptr<IImage2D> CloneImage2D() const override
			{
				return std::make_unique<BitmapImage>(m_Image.Copy());
			}

			// IImage2D: Create, save and load
			void Create(const Size& size);
			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			// IImage2D: Properties
			Size GetSize() const override
			{
				return m_Image.IsOk() ? Size(m_Image.GetSize()) : Size::UnspecifiedSize();
			}
			int GetWidth() const override
			{
				return m_Image.IsOk() ? m_Image.GetWidth() : Size::UnspecifiedSize().GetWidth();
			}
			int GetHeight() const override
			{
				return m_Image.IsOk() ? m_Image.GetWidth() : Size::UnspecifiedSize().GetHeight();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Image.HasAlpha() ? ColorDepthDB::BPP32 : ColorDepthDB::BPP24;
			}
			UniversallyUniqueID GetFormat() const override;

			// IImage2D: Options
			std::optional<String> GetOption(const String& name) const override;
			std::optional<int> GetOptionInt(const String& name) const override;

			void SetOption(const String& name, int value) override;
			void SetOption(const String& name, const String& value) override;

			// IImage2D: Conversion
			BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;
			GDIBitmap ToGDIBitmap(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;

		public:
			// IBitmapImage: Pixel data
			std::vector<PackedRGBA<uint8_t>> GetPixelDataRGBA() const override;
			std::vector<PackedRGB<uint8_t>> GetPixelDataRGB() const override;
			std::vector<uint8_t> GetPixelDataAlpha() const override;

			void SetPixelDataRGBA(const PackedRGBA<uint8_t>* pixelData) override;
			void SetPixelDataRGB(const PackedRGB<uint8_t>* pixelDataRGB) override;
			void SetPixelDataAlpha(const uint8_t* pixelDataAlpha) override;

			void ClearPixelDataRGBA(uint8_t value = 0) override;
			void ClearPixelDataRGB(uint8_t value = 0) override;
			void ClearPixelDataAlpha(uint8_t value = 0)override;

			PackedRGBA<uint8_t> GetPixelRGBA(const Point& pos) const override
			{
				return {GetRed(pos), GetGreen(pos), GetBlue(pos), GetAlpha(pos)};
			}
			PackedRGB<uint8_t> GetPixelRGB(const Point& pos) const override
			{
				return {GetRed(pos), GetGreen(pos), GetBlue(pos)};
			}

			void SetPixelRGBA(const Point& pos, const PackedRGBA<uint8_t>& color) override
			{
				InitAlpha();

				m_Image.SetRGB(pos.GetX(), pos.GetY(), color.Red, color.Green, color.Blue);
				m_Image.SetAlpha(pos.GetX(), pos.GetY(), color.Alpha);
			}
			void SetPixelRGB(const Point& pos, const PackedRGB<uint8_t>& color) override
			{
				m_Image.SetRGB(pos.GetX(), pos.GetY(), color.Red, color.Green, color.Blue);
			}
			void SetAreaRGBA(const Rect& rect, const PackedRGBA<uint8_t>& color) override
			{
				m_Image.SetRGB(rect, color.Red, color.Green, color.Blue);

				InitAlpha();
				for (int y = rect.GetY(); y < m_Image.GetHeight(); y++)
				{
					for (int x = rect.GetX(); x < m_Image.GetWidth(); x++)
					{
						m_Image.SetAlpha(x, y, color.Alpha);
					}
				}
			}
			void SetAreaRGB(const Rect& rect, const PackedRGB<uint8_t>& color) override
			{
				m_Image.SetRGB(rect, color.Red, color.Green, color.Blue);
			}

			void ReplaceRGB(const PackedRGB<uint8_t>& source, const PackedRGB<uint8_t>& target) override;
			void ReplaceRGBA(const PackedRGBA<uint8_t>& source, const PackedRGBA<uint8_t>& target) override;

			// IBitmapImage: Transparency
			bool IsPartiallyTransparent() const override;
			bool IsPixelTransparent(const Point& pos, uint8_t threshold = 128) const override;

		public:
			// BitmapImage
			void SetFormat(const UniversallyUniqueID& format);
			bool IsSameAs(const BitmapImage& other) const
			{
				return this == &other || m_Image.IsSameAs(other.m_Image);
			}
			BitmapImage Clone() const
			{
				return m_Image.Copy();
			}

			// BitmapImage: Conversion
			const wxImage& ToWxImage() const noexcept
			{
				return m_Image;
			}
			wxImage& ToWxImage() noexcept
			{
				return m_Image;
			}

			GDICursor ToGDICursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			GDIIcon ToGDIIcon() const;

			// BitmapImage: Pixel data
			const uint8_t* GetRawData() const
			{
				return m_Image.GetData();
			}
			uint8_t* GetRawData()
			{
				return m_Image.GetData();
			}
			void ClearRawData(uint8_t value = 0)
			{
				m_Image.Clear(value);
			}

			void SetRawData(wxMemoryBuffer& alpha)
			{
				m_Image.SetData(static_cast<unsigned char*>(alpha.release()), false);
			}
			void SetRawData(uint8_t* alpha)
			{
				m_Image.SetData(alpha, true);
			}
			void SetRawData(const Size& size, wxMemoryBuffer& alpha)
			{
				m_Image.SetData(static_cast<unsigned char*>(alpha.release()), size.GetWidth(), size.GetHeight(), false);
			}
			void SetRawData(const Size& size, uint8_t* alpha)
			{
				m_Image.SetData(alpha, size.GetWidth(), size.GetHeight(), true);
			}

			bool HasAlpha() const
			{
				return m_Image.HasAlpha();
			}
			bool InitAlpha()
			{
				if (!m_Image.HasAlpha())
				{
					m_Image.InitAlpha();
					return true;
				}
				return false;
			}
			bool ClearAlpha()
			{
				if (m_Image.HasAlpha())
				{
					m_Image.ClearAlpha();
					return true;
				}
				return false;
			}

			const uint8_t* GetRawAlpha() const
			{
				return m_Image.GetAlpha();
			}
			uint8_t* GetRawAlpha()
			{
				return m_Image.GetAlpha();
			}

			uint8_t GetRed(const Point& pos) const;
			uint8_t GetGreen(const Point& pos) const;
			uint8_t GetBlue(const Point& pos) const;
			uint8_t GetAlpha(const Point& pos) const;

			// BitmapImage: Transformation
			BitmapImage GetSubImage(const Rect& rect) const;

			BitmapImage Blur(int radius, Orientation orientation = Orientation::Both) const
			{
				switch (orientation)
				{
					case Orientation::Vertical:
					{
						return m_Image.BlurVertical(radius);
					}
					case Orientation::Horizontal:
					{
						return m_Image.BlurHorizontal(radius);
					}
					case Orientation::Both:
					{
						return m_Image.Blur(radius);
					}
				};
				return {};
			}
			BitmapImage Paste(const BitmapImage& image, const Point& pos, CompositionMode compositionMode = CompositionMode::Dest)
			{
				wxImage copy = m_Image;
				copy.Paste(image.m_Image, pos.GetX(), pos.GetY());
				return copy;
			}
			BitmapImage Mirror(Orientation orientation) const;
			BitmapImage Rotate(Angle angle, const Point& rotationCenter, InterpolationQuality interpolationQuality) const;

			BitmapImage& Resize(const Size& size, const Point& pos, const PackedRGB<uint8_t>& backgroundColor);
			BitmapImage& Rescale(const Size& size, InterpolationQuality interpolationQuality);

			// BitmapImage: Conversion
			BitmapImage ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const;
			BitmapImage ConvertToMonochrome(const PackedRGB<uint8_t>& makeWhite) const;
			BitmapImage ConvertToGrayscale(const PackedRGB<float>& weight = ColorWeight::CCIR_601) const;

			// BitmapImage: Misc
			std::optional<PackedRGB<uint8_t>> FindFirstUnusedColour(const PackedRGB<uint8_t>& startAt = {1, 0, 0}) const;
			BitmapImage& RotateHue(Angle angle);

			// BitmapImage: Mask
			bool IsMaskEnabled() const
			{
				return m_Image.HasMask();
			}
			void EnableMask(bool enable = true)
			{
				m_Image.SetMask(enable);
			}

			PackedRGB<uint8_t> GetMask() const
			{
				return {m_Image.GetMaskRed(), m_Image.GetMaskGreen(), m_Image.GetMaskBlue()};
			}
			void SetMask(const PackedRGB<uint8_t>& color)
			{
				m_Image.SetMaskColour(color.Red, color.Green, color.Blue);
			}
			void SetMask(const Color& color)
			{
				SetMask(color.GetFixed8().RemoveAlpha());
			}
			bool SetMask(const BitmapImage& shape, const PackedRGB<uint8_t>& color)
			{
				return m_Image.SetMaskFromImage(shape.m_Image, color.Red, color.Green, color.Blue);
			}
			bool SetMask(const BitmapImage& shape, const Color& color)
			{
				return SetMask(shape.m_Image, color.GetFixed8().RemoveAlpha());
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

			BitmapImage& operator=(const BitmapImage& other)
			{
				m_Image = other.m_Image;

				return *this;
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<BitmapImage> final
	{
		uint64_t Serialize(IOutputStream& stream, const BitmapImage& value) const
		{
			return value.Save(stream, ImageFormat::Any);
		}
		uint64_t Deserialize(IInputStream& stream, BitmapImage& value) const
		{
			return value.Load(stream, ImageFormat::Any);
		}
	};
}
