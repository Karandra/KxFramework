#include "stdafx.h"
#include "Cursor.h"
#include "Bitmap.h"
#include "Icon.h"
#include "Image.h"
#include "Private/GDI.h"

namespace kxf
{
	// Cursor
	Cursor::Cursor(const Icon& other)
		:m_Cursor(other.ToCursor().m_Cursor)
	{
	}
	Cursor::Cursor(const Bitmap& other)
		:m_Cursor(other.ToCursor().m_Cursor)
	{
	}
	Cursor::Cursor(const Image& other)
		:m_Cursor(other.ToCursor().m_Cursor)
	{
	}

	// IGDIObject
	void* Cursor::GetHandle() const
	{
		return m_Cursor.GetHandle();
	}
	void* Cursor::DetachHandle()
	{
		return Drawing::Private::DetachGDIImageHandle(m_Cursor);
	}
	void Cursor::AttachHandle(void* handle)
	{
		m_Cursor = wxCursor();
		if (!Drawing::Private::AttachIconHandle(m_Cursor, handle, [&]()
		{
			// This instance shouldn't be shared with anything at this point
			return true;
		}))
		{
			m_Cursor.SetHandle(handle);
		}
	}

	bool Cursor::Load(IInputStream& stream, Point hotSpot, ImageFormat format)
	{
		Image image;
		if (hotSpot.IsFullySpecified())
		{
			image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpot.GetX());
			image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpot.GetY());
		}

		if (image.Load(stream, format))
		{
			m_Cursor = std::move(image.ToCursor().m_Cursor);
			return m_Cursor.IsOk();
		}
		return false;
	}
	bool Cursor::Save(IOutputStream& stream, ImageFormat format)
	{
		if (m_Cursor.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			if (auto image = ToImage())
			{
				if (m_HotSpot.IsFullySpecified())
				{
					image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, m_HotSpot.GetX());
					image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, m_HotSpot.GetY());
				}
				return image.Save(stream, format);
			}
		}
		return false;
	}

	Bitmap Cursor::ToBitmap() const
	{
		return wxBitmap(m_Cursor);
	}
	Image Cursor::ToImage() const
	{
		return ToBitmap();
	}
	Icon Cursor::ToIcon() const
	{
		return ToBitmap();
	}



}

namespace kxf::Drawing
{
	Cursor GetStockCursor(StockCursor cursor)
	{
		return wxCursor(ToWxStockCursor(cursor));
	}
}
