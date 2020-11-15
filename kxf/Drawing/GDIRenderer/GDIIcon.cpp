#include "stdafx.h"
#include "GDIIcon.h"
#include "GDICursor.h"
#include "GDIBitmap.h"
#include "../Image.h"
#include "Private/GDI.h"

namespace kxf
{
	// Icon
	GDIIcon::GDIIcon(const GDICursor& other)
		:m_Icon(std::move(other.ToIcon().m_Icon))
	{
	}
	GDIIcon::GDIIcon(const Image& other)
		:m_Icon(std::move(other.ToIcon().m_Icon))
	{
	}
	GDIIcon::GDIIcon(const GDIBitmap& other)
		:m_Icon(std::move(other.ToIcon().m_Icon))
	{
	}

	// IGDIObject
	void* GDIIcon::GetHandle() const
	{
		return m_Icon.GetHandle();
	}
	void* GDIIcon::DetachHandle()
	{
		return Drawing::Private::DetachGDIImageHandle(m_Icon);
	}
	void GDIIcon::AttachHandle(void* handle)
	{
		m_Icon = wxIcon();
		Drawing::Private::AttachIconHandle(m_Icon, handle, [&]()
		{
			// This calls 'wxObject::AllocExclusive' inside
			m_Icon.CreateFromHICON(reinterpret_cast<WXHICON>(handle));
			return true;
		});
	}

	// IGDIImage
	bool GDIIcon::Load(IInputStream& stream, ImageFormat format, int index)
	{
		Image image;
		if (image.Load(stream, format, index))
		{
			m_Icon = std::move(image.ToIcon().m_Icon);
			return m_Icon.IsOk();
		}
		return false;
	}
	bool GDIIcon::Save(IOutputStream& stream, ImageFormat format) const
	{
		if (m_Icon.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			return ToImage().Save(stream, format);
		}
		return false;
	}

	// Icon
	GDICursor GDIIcon::ToCursor(const Point& hotSpot) const
	{
		GDICursor cursor(ToBitmap());
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	GDIBitmap GDIIcon::ToBitmap() const
	{
		return wxBitmap(m_Icon, wxBitmapTransparency::wxBitmapTransparency_Always);
	}
	Image GDIIcon::ToImage() const
	{
		return ToBitmap();
	}

	GDIIcon GDIIcon::ConvertToDisabled(Angle brightness) const
	{
		return ToBitmap().ConvertToDisabled(brightness);
	}
}
