#include "stdafx.h"
#include "Image.h"
#include "Bitmap.h"
#include "Cursor.h"
#include "Icon.h"
#include "kxf/IO/IStream.h"
#include "kxf/wxWidgets/StreamWrapper.h"

namespace kxf
{
	int Image::GetImageCount(IInputStream& stream, ImageFormat format)
	{
		wxWidgets::InputStreamWrapperWx warpper(stream);
		return wxImage::GetImageCount(warpper, static_cast<wxBitmapType>(format));
	}

	Image::Image(const Icon& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}
	Image::Image(const Bitmap& other)
		:m_Image(std::move(other.ToImage().m_Image))
	{
	}
	Image::Image(const Cursor& other)
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

	Cursor Image::ToCursor(const Point& hotSpot) const
	{
		wxCursor cursorWx(m_Image);
		Cursor cursor(std::move(cursorWx));
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	Bitmap Image::ToBitmap() const
	{
		return wxBitmap(m_Image, *ColorDepthDB::BPP32);
	}
	Icon Image::ToIcon() const
	{
		return ToBitmap();
	}
}
