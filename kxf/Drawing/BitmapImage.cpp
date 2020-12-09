#include "stdafx.h"
#include "BitmapImage.h"
#include "GDIRenderer/GDIBitmap.h"
#include "GDIRenderer/GDICursor.h"
#include "GDIRenderer/GDIIcon.h"
#include "kxf/IO/IStream.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include "Private/SVGImageHandler.h"

namespace
{
	constexpr uint8_t g_OpaqueAlpha = kxf::ColorTraits<uint8_t>::max();
	constexpr size_t g_ChannelsRGB = 3;
	constexpr size_t g_ChannelsRGBA = g_ChannelsRGB + 1;

	size_t GetImageBufferSize(const wxImage& image, size_t dimensions = 1) noexcept
	{
		return static_cast<size_t>(image.GetWidth()) * static_cast<size_t>(image.GetHeight()) * dimensions;
	}
	size_t GetImageBufferSizeAuto(const wxImage& image) noexcept
	{
		return GetImageBufferSize(image, image.HasAlpha() ? g_ChannelsRGBA : g_ChannelsRGB);
	}

	constexpr void AssertPackedColorLayout()
	{
		using namespace kxf;

		static_assert(sizeof(PackedRGB<uint8_t>) == 3 && alignof(PackedRGB<uint8_t>) == alignof(uint8_t), "Invalid layout of PackedRGB<uint8_t> structure");
		static_assert(sizeof(PackedRGBA<uint8_t>) == 4 && alignof(PackedRGBA<uint8_t>) == alignof(uint8_t), "Invalid layout of PackedRGBA<uint8_t> structure");
	}
}

namespace kxf
{
	// BitmapImage
	void BitmapImage::InitalizeHandlers()
	{
		wxInitAllImageHandlers();
		wxImage::AddHandler(std::make_unique<Drawing::Private::SVGImageHandler>().release());
	}
	size_t BitmapImage::GetImageCount(IInputStream& stream, const UniversallyUniqueID& format)
	{
		wxWidgets::InputStreamWrapperWx warpper(stream);
		return wxImage::GetImageCount(warpper, Drawing::Private::MapImageFormat(format));
	}

	BitmapImage::BitmapImage(const GDIIcon& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}
	BitmapImage::BitmapImage(const GDIBitmap& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}
	BitmapImage::BitmapImage(const GDICursor& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}

	// IImage2D: Create, save and load
	void BitmapImage::Create(const Size& size)
	{
		m_Image.Create(size, false);
	}
	bool BitmapImage::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		wxWidgets::InputStreamWrapperWx warpper(stream);
		return m_Image.LoadFile(warpper, Drawing::Private::MapImageFormat(format), index == IImage2D::npos ? -1 : static_cast<int>(index));
	}
	bool BitmapImage::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
	{
		if (m_Image.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			wxWidgets::OutputStreamWrapperWx warpper(stream);
			return m_Image.SaveFile(warpper, Drawing::Private::MapImageFormat(format));
		}
		return false;
	}

	// IImage2D: Properties
	UniversallyUniqueID BitmapImage::GetFormat() const
	{
		return Drawing::Private::MapImageFormat(m_Image.GetType());
	}

	// IImage2D: Options
	std::optional<String> BitmapImage::GetOption(const String& name) const
	{
		if (m_Image.HasOption(name))
		{
			return m_Image.GetOption(name);
		}
		return {};
	}
	std::optional<int> BitmapImage::GetOptionInt(const String& name) const
	{
		if (m_Image.HasOption(name))
		{
			return m_Image.GetOptionInt(name);
		}
		else if (name == ImageOption::DPI)
		{
			if (m_Image.GetOptionInt(ImageOption::ResolutionUnit) == wxIMAGE_RESOLUTION_NONE)
			{
				if (m_Image.HasOption(ImageOption::Resolution))
				{
					return m_Image.GetOptionInt(ImageOption::Resolution);
				}
				else if (m_Image.HasOption(ImageOption::ResolutionX) || m_Image.GetOptionInt(ImageOption::ResolutionY))
				{
					return std::min(m_Image.GetOptionInt(ImageOption::ResolutionX), m_Image.GetOptionInt(ImageOption::ResolutionY));
				}
			}
		}
		return {};
	}

	void BitmapImage::SetOption(const String& name, int value)
	{
		if (name == ImageOption::DPI)
		{
			m_Image.SetOption(ImageOption::ResolutionUnit, wxIMAGE_RESOLUTION_NONE);
			m_Image.SetOption(ImageOption::Resolution, value);
			m_Image.SetOption(ImageOption::ResolutionX, value);
			m_Image.SetOption(ImageOption::ResolutionY, value);

			return;
		}
		m_Image.SetOption(name, value);
	}
	void BitmapImage::SetOption(const String& name, const String& value)
	{
		m_Image.SetOption(name, value);
	}

	// IBitmapImage: Pixel data
	std::vector<PackedRGBA<uint8_t>> BitmapImage::GetPixelDataRGBA() const
	{
		if (m_Image.IsOk())
		{
			std::vector<PackedRGBA<uint8_t>> bufferRGBA;
			bufferRGBA.resize(GetImageBufferSize(m_Image));

			const auto rgb = m_Image.GetData();
			const auto alpha = m_Image.GetAlpha();
			for (size_t i = 0; i < bufferRGBA.size(); i++)
			{
				const auto pixel = rgb + i;
				bufferRGBA[i] = {*pixel, *(pixel + 1), *(pixel + 2), alpha ? *(alpha + i) : g_OpaqueAlpha};
			}

			return bufferRGBA;
		}
		return {};
	}
	std::vector<PackedRGB<uint8_t>> BitmapImage::GetPixelDataRGB() const
	{
		AssertPackedColorLayout();

		if (m_Image.IsOk())
		{
			std::vector<PackedRGB<uint8_t>> bufferRGB;
			bufferRGB.resize(GetImageBufferSize(m_Image));
			std::memcpy(bufferRGB.data(), m_Image.GetData(), GetImageBufferSize(m_Image, 3));

			return bufferRGB;
		}
		return {};
	}
	std::vector<uint8_t> BitmapImage::GetPixelDataAlpha() const
	{
		if (m_Image.IsOk() && m_Image.HasAlpha())
		{
			const auto alpha = m_Image.GetAlpha();
			return {alpha, alpha + GetImageBufferSize(m_Image)};
		}
		return {};
	}

	void BitmapImage::SetPixelDataRGBA(const PackedRGBA<uint8_t>* pixelData)
	{
		if (m_Image.IsOk())
		{
			if (!m_Image.HasAlpha())
			{
				m_Image.InitAlpha();
			}

			auto targetRGB = m_Image.GetData();
			auto targetAlpha = m_Image.GetAlpha();

			const size_t totalSize = GetImageBufferSize(m_Image);
			for (size_t i = 0; i < totalSize; i++)
			{
				// Copy first 3 components for RGB data
				auto rgb = targetRGB + (i * 3);
				rgb[0] = pixelData[i].Red;
				rgb[1] = pixelData[i].Green;
				rgb[2] = pixelData[i].Blue;

				// Copy single component for alpha data
				targetAlpha[i] = pixelData[i].Alpha;
			}
		}
	}
	void BitmapImage::SetPixelDataRGB(const PackedRGB<uint8_t>* pixelDataRGB)
	{
		if (m_Image.IsOk())
		{
			AssertPackedColorLayout();

			std::memcpy(m_Image.GetData(), pixelDataRGB, GetImageBufferSize(m_Image, 3));
		}
	}
	void BitmapImage::SetPixelDataAlpha(const uint8_t* pixelDataAlpha)
	{
		if (m_Image.IsOk())
		{
			if (!m_Image.HasAlpha())
			{
				m_Image.InitAlpha();
			}
			std::memcpy(m_Image.GetAlpha(), pixelDataAlpha, GetImageBufferSize(m_Image));
		}
	}

	void BitmapImage::ClearPixelDataRGBA(uint8_t value)
	{
		m_Image.Clear(value);
		if (!m_Image.HasAlpha())
		{
			m_Image.InitAlpha();
		}
		std::memset(m_Image.GetAlpha(), value, GetImageBufferSize(m_Image));
	}
	void BitmapImage::ClearPixelDataRGB(uint8_t value)
	{
		m_Image.Clear(value);
	}
	void BitmapImage::ClearPixelDataAlpha(uint8_t value)
	{
		if (!m_Image.HasAlpha())
		{
			m_Image.InitAlpha();
		}
		std::memset(m_Image.GetAlpha(), value, GetImageBufferSize(m_Image));
	}

	void BitmapImage::ReplaceRGB(const PackedRGB<uint8_t>& source, const PackedRGB<uint8_t>& target)
	{
		m_Image.Replace(source.Red, source.Green, source.Blue, target.Red, target.Green, target.Blue);
	}
	void BitmapImage::ReplaceRGBA(const PackedRGBA<uint8_t>& source, const PackedRGBA<uint8_t>& target)
	{
		if (m_Image.IsOk())
		{
			if (!m_Image.HasAlpha())
			{
				m_Image.InitAlpha();
			}

			auto rgb = m_Image.GetData();
			auto alpha = m_Image.GetAlpha();
			const size_t totalSize = GetImageBufferSize(m_Image);
			for (size_t i = 0; i < totalSize; i++)
			{
				auto pixel = rgb + (i * 3);
				if (source.Red == pixel[0] && source.Green == pixel[1] && source.Blue == pixel[2] && source.Alpha == alpha[i])
				{
					pixel[0] = target.Red;
					pixel[1] = target.Green;
					pixel[2] = target.Blue;
					alpha[i] = target.Alpha;
				}
			}
		}
	}

	// IBitmapImage: Transparency
	bool BitmapImage::IsPartiallyTransparent() const
	{
		if (m_Image.HasAlpha())
		{
			const auto alpha = m_Image.GetAlpha();
			const size_t count = GetImageBufferSize(m_Image);
			for (size_t i = 0; i < count; i++)
			{
				if (alpha[i] != g_OpaqueAlpha)
				{
					return true;
				}
			}
		}
		return false;
	}
	bool BitmapImage::IsPixelTransparent(const Point& pos, uint8_t threshold) const
	{
		return m_Image.IsTransparent(pos.GetX(), pos.GetY(), threshold);
	}

	// BitmapImage
	void BitmapImage::SetFormat(const UniversallyUniqueID& format)
	{
		m_Image.SetType(Drawing::Private::MapImageFormat(format));
	}

	GDICursor BitmapImage::ToCursor(const Point& hotSpot) const
	{
		wxCursor cursorWx(m_Image);
		GDICursor cursor(std::move(cursorWx));
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	GDIBitmap BitmapImage::ToBitmap() const
	{
		return wxBitmap(m_Image, *ColorDepthDB::BPP32);
	}
	GDIIcon BitmapImage::ToIcon() const
	{
		return ToBitmap();
	}

	// BitmapImage: Pixel data
	uint8_t BitmapImage::GetRed(const Point& pos) const
	{
		return m_Image.GetRed(pos.GetX(), pos.GetY());
	}
	uint8_t BitmapImage::GetGreen(const Point& pos) const
	{
		return m_Image.GetGreen(pos.GetX(), pos.GetY());
	}
	uint8_t BitmapImage::GetBlue(const Point& pos) const
	{
		return m_Image.GetBlue(pos.GetX(), pos.GetY());
	}
	uint8_t BitmapImage::GetAlpha(const Point& pos) const
	{
		return m_Image.HasAlpha() ? m_Image.GetAlpha(pos.GetX(), pos.GetY()) : g_OpaqueAlpha;
	}

	// BitmapImage: Transformation
	BitmapImage BitmapImage::GetSubImage(const Rect& rect) const
	{
		return m_Image.GetSubImage(rect);
	}
	BitmapImage BitmapImage::Mirror(Orientation orientation) const
	{
		if (m_Image.IsOk())
		{
			switch (orientation)
			{
				case Orientation::Vertical:
				{
					return m_Image.Mirror(false);
				}
				case Orientation::Horizontal:
				{
					return m_Image.Mirror(true);
				}
			};
		}
		return {};
	}
	BitmapImage BitmapImage::Rotate(Angle angle, const Point& rotationCenter, InterpolationQuality interpolationQuality) const
	{
		if (m_Image.IsOk())
		{
			const float deg = angle.ToDegrees();

			if (deg == 90.0f)
			{
				return m_Image.Rotate90(true);
			}
			else if (deg == -90.0f)
			{
				return m_Image.Rotate90(false);
			}
			else if (deg == 180.0f)
			{
				return m_Image.Rotate180();
			}
			else
			{
				return m_Image.Rotate(angle.ToRadians(), rotationCenter, interpolationQuality != InterpolationQuality::None);
			}
		}
		return {};
	}

	BitmapImage& BitmapImage::Resize(const Size& size, const Point& pos, const PackedRGB<uint8_t>& backgroundColor)
	{
		m_Image.Resize(size, pos, backgroundColor.Red, backgroundColor.Green, backgroundColor.Blue);
		return *this;
	}
	BitmapImage& BitmapImage::Rescale(const Size& size, InterpolationQuality interpolationQuality)
	{
		auto DoScale = [&](wxImageResizeQuality quality)
		{
			m_Image.Rescale(size.GetWidth(), size.GetHeight(), quality);
		};

		switch (interpolationQuality)
		{
			case InterpolationQuality::Default:
			case InterpolationQuality::FastestAvailable:
			{
				DoScale(wxIMAGE_QUALITY_NORMAL);
				break;
			}
			case InterpolationQuality::BestAvailable:
			{
				DoScale(wxIMAGE_QUALITY_HIGH);
				break;
			}
			case InterpolationQuality::NearestNeighbor:
			{
				DoScale(wxIMAGE_QUALITY_NEAREST);
				break;
			}
			case InterpolationQuality::Bilinear:
			{
				DoScale(wxIMAGE_QUALITY_BILINEAR);
				break;
			}
			case InterpolationQuality::Bicubic:
			{
				DoScale(wxIMAGE_QUALITY_BICUBIC);
				break;
			}
			case InterpolationQuality::BoxAverage:
			{
				DoScale(wxIMAGE_QUALITY_BOX_AVERAGE);
				break;
			}
		};
		return *this;
	}

	// BitmapImage: Conversion
	BitmapImage BitmapImage::ConvertToDisabled(Angle brightness) const
	{
		if (m_Image.IsOk())
		{
			uint8_t value = brightness.ToNormalized() * ColorTraits<uint8_t>::max();
			return m_Image.ConvertToDisabled(value);
		}
		return {};
	}
	BitmapImage BitmapImage::ConvertToMonochrome(const PackedRGB<uint8_t>& makeWhite) const
	{
		if (m_Image.IsOk())
		{
			return m_Image.ConvertToMono(makeWhite.Red, makeWhite.Green, makeWhite.Blue);
		}
		return {};
	}
	BitmapImage BitmapImage::ConvertToGrayscale(const PackedRGB<float>& weight) const
	{
		if (m_Image.IsOk())
		{
			return m_Image.ConvertToGreyscale(weight.Red, weight.Green, weight.Blue);
		}
		return {};
	}

	// BitmapImage: Misc
	std::optional<PackedRGB<uint8_t>> BitmapImage::FindFirstUnusedColour(const PackedRGB<uint8_t>& startAt) const
	{
		PackedRGB<uint8_t> firstUnused;
		if (m_Image.IsOk() && m_Image.FindFirstUnusedColour(&firstUnused.Red, &firstUnused.Green, &firstUnused.Blue, startAt.Red, startAt.Green, startAt.Blue))
		{
			return firstUnused;
		}
		return {};
	}
	BitmapImage& BitmapImage::RotateHue(Angle angle)
	{
		m_Image.RotateHue(angle.ToNormalized());
		return *this;
	}
}
