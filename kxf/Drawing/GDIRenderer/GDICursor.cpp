#include "stdafx.h"
#include "GDICursor.h"
#include "GDIBitmap.h"
#include "GDIIcon.h"
#include "../Image.h"
#include "Private/GDI.h"

namespace kxf
{
	// GDICursor
	GDICursor::GDICursor(const GDIIcon& other)
		:m_Cursor(other.ToCursor().m_Cursor)
	{
	}
	GDICursor::GDICursor(const GDIBitmap& other)
		:m_Cursor(other.ToCursor().m_Cursor)
	{
	}
	GDICursor::GDICursor(const Image& other)
		:m_Cursor(other.ToCursor().m_Cursor)
	{
	}

	// IGDIObject
	void* GDICursor::GetHandle() const
	{
		return m_Cursor.GetHandle();
	}
	void* GDICursor::DetachHandle()
	{
		return Drawing::Private::DetachGDIImageHandle(m_Cursor);
	}
	void GDICursor::AttachHandle(void* handle)
	{
		m_Cursor = wxCursor();
		Drawing::Private::AttachIconHandle(m_Cursor, handle, [&]()
		{
			m_Cursor = wxStockCursor::wxCURSOR_ARROW;
			return true;
		});
	}

	bool GDICursor::Load(IInputStream& stream, Point hotSpot, ImageFormat format)
	{
		Image image;
		if (hotSpot.IsFullySpecified())
		{
			image.SetOption(ImageOption::Cursor::HotSpotX, hotSpot.GetX());
			image.SetOption(ImageOption::Cursor::HotSpotY, hotSpot.GetY());
		}

		if (image.Load(stream, format))
		{
			m_Cursor = std::move(image.ToCursor().m_Cursor);
			return m_Cursor.IsOk();
		}
		return false;
	}
	bool GDICursor::Save(IOutputStream& stream, ImageFormat format) const
	{
		if (m_Cursor.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			if (auto image = ToImage())
			{
				if (m_HotSpot.IsFullySpecified())
				{
					image.SetOption(ImageOption::Cursor::HotSpotX, m_HotSpot.GetX());
					image.SetOption(ImageOption::Cursor::HotSpotY, m_HotSpot.GetY());
				}
				return image.Save(stream, format);
			}
		}
		return false;
	}

	GDIBitmap GDICursor::ToBitmap() const
	{
		return wxBitmap(m_Cursor);
	}
	Image GDICursor::ToImage() const
	{
		return ToBitmap();
	}
	GDIIcon GDICursor::ToIcon() const
	{
		return ToBitmap();
	}
}

namespace kxf::Drawing
{
	GDICursor GetStockCursor(StockCursor cursor)
	{
		return wxCursor(ToWxStockCursor(cursor));
	}
}
