#include "stdafx.h"
#include "GDIIcon.h"
#include "GDICursor.h"
#include "GDIBitmap.h"
#include "../BitmapImage.h"
#include "Private/GDI.h"

namespace kxf
{
	// Icon
	GDIIcon::GDIIcon(const GDICursor& other)
		:m_Icon(std::move(other.ToIcon().m_Icon))
	{
	}
	GDIIcon::GDIIcon(const BitmapImage& other)
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

	void GDIIcon::Create(const Size& size)
	{
		BitmapImage image(size);
		m_Icon = std::move(image.ToIcon().m_Icon);
	}

	// IImage2D
	bool GDIIcon::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		BitmapImage image;
		if (image.Load(stream, format, index == IImage2D::npos ? -1 : static_cast<int>(index)))
		{
			m_Icon = std::move(image.ToIcon().m_Icon);
			return m_Icon.IsOk();
		}
		return false;
	}
	bool GDIIcon::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
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
	BitmapImage GDIIcon::ToImage() const
	{
		return ToBitmap();
	}

	GDIIcon GDIIcon::ConvertToDisabled(Angle brightness) const
	{
		return ToBitmap().ConvertToDisabled(brightness);
	}
}
