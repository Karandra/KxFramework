#include "stdafx.h"
#include "GDIBitmap.h"
#include "GDICursor.h"
#include "GDIIcon.h"
#include "../Image.h"
#include "../GDIRenderer/GDIContext.h"
#include "Private/GDI.h"

namespace kxf
{
	// GDIBitmap
	void GDIBitmap::Initialize()
	{
		// This shouldn't be necessary, at least for 'ToImage' function
		// since it can detect whether the alpha is actually used or not.
		//m_Bitmap.UseAlpha(true);
	}

	GDIBitmap::GDIBitmap(const GDICursor& other)
		:m_Bitmap(std::move(other.ToBitmap().m_Bitmap))
	{
		Initialize();
	}
	GDIBitmap::GDIBitmap(const GDIIcon& other)
		:m_Bitmap(std::move(other.ToBitmap().m_Bitmap))
	{
		Initialize();
	}
	GDIBitmap::GDIBitmap(const Image& other)
		:m_Bitmap(std::move(other.ToBitmap().m_Bitmap))
	{
		Initialize();
	}
	GDIBitmap::GDIBitmap(const Image& other, const GDIContext& dc)
		:m_Bitmap(other.ToWxImage(), dc.ToWxDC())
	{
		Initialize();
	}
	GDIBitmap::GDIBitmap(const Size& size, const GDIContext& dc)
		:m_Bitmap(size.GetWidth(), size.GetHeight(), dc.ToWxDC())
	{
		Initialize();
	}

	// IGDIObject
	void* GDIBitmap::GetHandle() const
	{
		return m_Bitmap.GetHandle();
	}
	void* GDIBitmap::DetachHandle()
	{
		return Drawing::Private::DetachGDIImageHandle(m_Bitmap);
	}
	void GDIBitmap::AttachHandle(void* handle)
	{
		m_Bitmap = wxBitmap();
		Initialize();

		if (handle)
		{
			BITMAP bitmap = {};
			if (::GetObject(handle, sizeof(bitmap), &bitmap) != 0)
			{
				m_Bitmap.InitFromHBITMAP(reinterpret_cast<WXHBITMAP>(handle), bitmap.bmWidth, bitmap.bmHeight, bitmap.bmBitsPixel);
				return;
			}
		}
		m_Bitmap.SetHandle(handle);
	}

	// IGDIImage
	bool GDIBitmap::Load(IInputStream& stream, ImageFormat format)
	{
		Image image;
		if (image.Load(stream, format))
		{
			m_Bitmap = std::move(image.ToBitmap().m_Bitmap);
			return m_Bitmap.IsOk();
		}
		return false;
	}
	bool GDIBitmap::Save(IOutputStream& stream, ImageFormat format) const
	{
		if (m_Bitmap.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			return ToImage().Save(stream, format);
		}
		return false;
	}

	// GDIBitmap
	GDICursor GDIBitmap::ToCursor(const Point& hotSpot) const
	{
		GDICursor cursor(wxCursor(m_Bitmap.ConvertToImage()));
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	Image GDIBitmap::ToImage() const
	{
		return m_Bitmap.ConvertToImage();
	}
	GDIIcon GDIBitmap::ToIcon() const
	{
		wxIcon icon;
		icon.CopyFromBitmap(m_Bitmap);

		return icon;
	}
}
