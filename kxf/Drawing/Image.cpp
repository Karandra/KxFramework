#include "stdafx.h"
#include "Image.h"
#include "GDIRenderer/GDIBitmap.h"
#include "GDIRenderer/GDICursor.h"
#include "GDIRenderer/GDIIcon.h"
#include "kxf/IO/IStream.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include "Private/SVGImageHandler.h"

namespace kxf
{
	void Image::InitalizeHandlers()
	{
		wxInitAllImageHandlers();
		wxImage::AddHandler(std::make_unique<Drawing::Private::SVGImageHandler>().release());
	}
	size_t Image::GetImageCount(IInputStream& stream, ImageFormat format)
	{
		wxWidgets::InputStreamWrapperWx warpper(stream);
		return wxImage::GetImageCount(warpper, static_cast<wxBitmapType>(format));
	}

	Image::Image(const GDIIcon& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}
	Image::Image(const GDIBitmap& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}
	Image::Image(const GDICursor& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}

	bool Image::Load(IInputStream& stream, ImageFormat format, int index)
	{
		wxWidgets::InputStreamWrapperWx warpper(stream);
		return m_Image.LoadFile(warpper, static_cast<wxBitmapType>(format), index);
	}
	bool Image::Save(IOutputStream& stream, ImageFormat format) const
	{
		if (m_Image.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			wxWidgets::OutputStreamWrapperWx warpper(stream);
			return m_Image.SaveFile(warpper, static_cast<wxBitmapType>(format));
		}
		return false;
	}

	GDICursor Image::ToCursor(const Point& hotSpot) const
	{
		wxCursor cursorWx(m_Image);
		GDICursor cursor(std::move(cursorWx));
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	GDIBitmap Image::ToBitmap() const
	{
		return wxBitmap(m_Image, *ColorDepthDB::BPP32);
	}
	GDIIcon Image::ToIcon() const
	{
		return ToBitmap();
	}
}
