#include "stdafx.h"
#include "GDICursor.h"
#include "GDIBitmap.h"
#include "GDIIcon.h"
#include "../BitmapImage.h"
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
	GDICursor::GDICursor(const BitmapImage& other)
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

	void GDICursor::Create(const Size& size)
	{
		BitmapImage image(size);
		if (m_HotSpot.IsFullySpecified())
		{
			image.SetOption(ImageOption::Cursor::HotSpotX, m_HotSpot.GetX());
			image.SetOption(ImageOption::Cursor::HotSpotY, m_HotSpot.GetY());
		}
		m_Cursor = std::move(image.ToCursor().m_Cursor);
	}

	// IImage2D
	bool GDICursor::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		BitmapImage image;
		if (m_HotSpot.IsFullySpecified())
		{
			image.SetOption(ImageOption::Cursor::HotSpotX, m_HotSpot.GetX());
			image.SetOption(ImageOption::Cursor::HotSpotY, m_HotSpot.GetY());
		}

		if (image.Load(stream, format))
		{
			m_Cursor = std::move(image.ToCursor().m_Cursor);
			return m_Cursor.IsOk();
		}
		return false;
	}
	bool GDICursor::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
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

	std::optional<int> GDICursor::GetOptionInt(const String& name) const
	{
		if (name == ImageOption::Cursor::HotSpotX)
		{
			return m_HotSpot.GetX();
		}
		else if (name == ImageOption::Cursor::HotSpotX)
		{
			return m_HotSpot.GetY();
		}
		return {};
	}
	void GDICursor::SetOption(const String& name, int value)
	{
		if (name == ImageOption::Cursor::HotSpotX)
		{
			m_HotSpot.SetX(value);
		}
		else if (name == ImageOption::Cursor::HotSpotX)
		{
			m_HotSpot.SetY(value);
		}
	}

	// GDICursor
	GDIBitmap GDICursor::ToBitmap() const
	{
		return wxBitmap(m_Cursor);
	}
	BitmapImage GDICursor::ToImage() const
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
