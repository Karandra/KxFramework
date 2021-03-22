#include "KxfPCH.h"
#include "GDIBitmap.h"
#include "GDICursor.h"
#include "GDIIcon.h"
#include "../BitmapImage.h"
#include "../GDIRenderer/GDIContext.h"
#include "Private/GDI.h"

namespace kxf
{
	// GDIBitmap
	void GDIBitmap::Initialize()
	{
		// This shouldn't be necessary, at least for 'ToBitmapImage' function
		// since it can detect whether the alpha is actually used or not.
		//m_Bitmap.UseAlpha(true);
	}

	GDIBitmap::GDIBitmap(const GDICursor& other)
		:m_Bitmap(std::move(other.ToGDIBitmap().m_Bitmap))
	{
		Initialize();
	}
	GDIBitmap::GDIBitmap(const GDIIcon& other)
		:m_Bitmap(std::move(other.ToGDIBitmap().m_Bitmap))
	{
		Initialize();
	}
	GDIBitmap::GDIBitmap(const BitmapImage& other)
		:m_Bitmap(std::move(other.ToGDIBitmap().m_Bitmap))
	{
		Initialize();
	}
	GDIBitmap::GDIBitmap(const BitmapImage& other, const GDIContext& dc)
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

	// IImage2D
	bool GDIBitmap::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		BitmapImage image;
		if (image.Load(stream, format))
		{
			m_Bitmap = std::move(image.ToGDIBitmap().m_Bitmap);
			return m_Bitmap.IsOk();
		}
		return false;
	}
	bool GDIBitmap::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
	{
		if (m_Bitmap.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			return ToBitmapImage().Save(stream, format);
		}
		return false;
	}

	BitmapImage GDIBitmap::ToBitmapImage(const Size& size, InterpolationQuality interpolationQuality) const
	{
		if (m_Bitmap.IsOk())
		{
			if (!size.IsFullySpecified() || m_Bitmap.GetSize() == size)
			{
				return m_Bitmap.ConvertToImage();
			}
			else
			{
				BitmapImage image = m_Bitmap.ConvertToImage();
				image.Rescale(size, interpolationQuality);
				return image.ToGDIBitmap();
			}
		}
		return {};
	}
	GDIBitmap GDIBitmap::ToGDIBitmap(const Size& size, InterpolationQuality interpolationQuality) const
	{
		if (m_Bitmap.IsOk())
		{
			if (!size.IsFullySpecified() || m_Bitmap.GetSize() == size)
			{
				return m_Bitmap;
			}
			else
			{
				BitmapImage image = m_Bitmap.ConvertToImage();
				image.Rescale(size, interpolationQuality);
				return image.ToGDIBitmap();
			}
		}
		return {};
	}

	// GDIBitmap
	GDICursor GDIBitmap::ToGDICursor(const Point& hotSpot) const
	{
		GDICursor cursor(wxCursor(m_Bitmap.ConvertToImage()));
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	GDIIcon GDIBitmap::ToGDIIcon() const
	{
		wxIcon icon;
		icon.CopyFromBitmap(m_Bitmap);

		return icon;
	}
}
