#include "stdafx.h"
#include "Icon.h"
#include "Cursor.h"
#include "Bitmap.h"
#include "Image.h"
#include "Private/GDI.h"

namespace kxf
{
	// Icon
	Icon::Icon(const Cursor& other)
		:m_Icon(std::move(other.ToIcon().m_Icon))
	{
	}
	Icon::Icon(const Image& other)
		:m_Icon(std::move(other.ToIcon().m_Icon))
	{
	}
	Icon::Icon(const Bitmap& other)
		:m_Icon(std::move(other.ToIcon().m_Icon))
	{
	}

	// IGDIObject
	void* Icon::GetHandle() const
	{
		return m_Icon.GetHandle();
	}
	void* Icon::DetachHandle()
	{
		return Drawing::Private::DetachGDIImageHandle(m_Icon);
	}
	void Icon::AttachHandle(void* handle)
	{
		m_Icon = wxIcon();
		if (!Drawing::Private::AttachIconHandle(m_Icon, handle, [&]()
		{
			// This calls 'wxObject::AllocExclusive' inside
			m_Icon.CreateFromHICON(reinterpret_cast<WXHICON>(handle));
			return true;
		}))
		{
			m_Icon.SetHandle(handle);
		}
	}

	// IGDIImage
	bool Icon::Load(IInputStream& stream, ImageFormat format, int index)
	{
		Image image;
		if (image.Load(stream, format, index))
		{
			m_Icon = std::move(image.ToIcon().m_Icon);
			return m_Icon.IsOk();
		}
		return false;
	}
	bool Icon::Save(IOutputStream& stream, ImageFormat format) const
	{
		if (m_Icon.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			return ToImage().Save(stream, format);
		}
		return false;
	}

	// Icon
	Cursor Icon::ToCursor(const Point& hotSpot) const
	{
		Cursor cursor(ToBitmap());
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	Bitmap Icon::ToBitmap() const
	{
		return wxBitmap(m_Icon, wxBitmapTransparency::wxBitmapTransparency_Always);
	}
	Image Icon::ToImage() const
	{
		return ToBitmap();
	}

	Icon Icon::ConvertToDisabled(Angle brightness) const
	{
		return ToBitmap().ConvertToDisabled(brightness);
	}
}
