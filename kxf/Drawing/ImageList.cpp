#include "stdafx.h"
#include "ImageList.h"
#include "GDIRenderer.h"
#include "kxf/System/COM.h"
#include "kxf/Utility/Common.h"

#include <Shlobj.h>
#include <commoncontrols.h>
#include "kxf/System/UndefWindows.h"

namespace
{
	using namespace kxf;

	constexpr bool g_UseMask = false;

	constexpr uint32_t MapDrawMode(FlagSet<ImageListFlag> drawMode) noexcept
	{
		uint32_t nativeDrawMode = ILD_IMAGE|ILD_NORMAL;
		Utility::AddFlagRef(nativeDrawMode, ILD_TRANSPARENT, drawMode & ImageListFlag::Transparent);
		Utility::AddFlagRef(nativeDrawMode, ILD_SELECTED, drawMode & ImageListFlag::Selected);
		Utility::AddFlagRef(nativeDrawMode, ILD_FOCUS, drawMode & ImageListFlag::Focused);

		return nativeDrawMode;
	}
	HIMAGELIST ToHImageList(WXHIMAGELIST handle) noexcept
	{
		return reinterpret_cast<HIMAGELIST>(handle);
	}
}

namespace kxf
{
	wxIMPLEMENT_DYNAMIC_CLASS(ImageList, wxImageList);

	// ImageList
	void ImageList::OnCreate(int width, int height, bool mask, int initialCount) noexcept
	{
		// Replicate wxImageList flags algorithm
		m_Flags = ILC_COLOR32;

		// Common Controls before v6 always uses masks as it doesn't support alpha
		if (mask || wxApp::GetComCtl32Version() < 600)
		{
			m_Flags |= ILC_MASK;
		}
	}
	bool ImageList::DoDraw(GDIContext& dc, int index, const Rect& rect, FlagSet<ImageListFlag> flags, int overlayIndex) noexcept
	{
		Size size = rect.GetSize();
		size.SetDefaults({0, 0});

		uint32_t nativeDrawMode = MapDrawMode(flags);
		if (overlayIndex > 0)
		{
			nativeDrawMode |= INDEXTOOVERLAYMASK(overlayIndex);
		}

		return ::ImageList_DrawEx(ToHImageList(m_hImageList), index, static_cast<HDC>(dc.GetHandle()), rect.GetX(), rect.GetY(), size.GetWidth(), size.GetHeight(), CLR_NONE, CLR_NONE, nativeDrawMode);
	}

	ImageList::ImageList() noexcept
	{
		OnCreate(wxDefaultCoord, wxDefaultCoord, g_UseMask, -1);
	}
	ImageList::ImageList(int width, int height, int initialCount) noexcept
		:wxImageList(width, height, g_UseMask, initialCount)
	{
		OnCreate(width, height, g_UseMask, initialCount);
	}

	// IGDIObject
	bool ImageList::IsNull() const
	{
		return m_hImageList != nullptr;
	}
	bool ImageList::IsSameAs(const IGDIObject& other) const
	{
		return m_hImageList == other.GetHandle();
	}
	std::unique_ptr<kxf::IGDIObject> ImageList::Clone() const
	{
		const size_t count = GetImageCount();
		auto clone = std::make_unique<ImageList>(GetSize(), count);
		clone->m_Flags = m_Flags;

		for (size_t i = 0; i < count; i++)
		{
			clone->Add(GetIcon(i));
		}
		return clone;
	}

	void* ImageList::GetHandle() const
	{
		return m_hImageList;
	}
	void* ImageList::DetachHandle()
	{
		void* handle = m_hImageList;
		m_hImageList = nullptr;
		m_size = wxDefaultSize;

		return handle;
	}
	void ImageList::AttachHandle(void* handle)
	{
		AllocExclusive();

		if (m_hImageList)
		{
			::ImageList_Destroy(ToHImageList(m_hImageList));
			m_hImageList = nullptr;
		}

		if (handle)
		{
			m_hImageList = static_cast<WXHIMAGELIST>(handle);
			if (m_hImageList)
			{
				::ImageList_GetIconSize(ToHImageList(m_hImageList), &m_size.x, &m_size.y);
			}
		}
		else
		{
			m_size = wxDefaultSize;
		}
	}

	// ImageList
	bool ImageList::HasMask() const noexcept
	{
		return m_Flags & ILC_MASK;
	}
	COMPtr<IImageList2> ImageList::QueryNativeInterface() const noexcept
	{
		COMPtr<IImageList2> imageList;
		if (HResult(::HIMAGELIST_QueryInterface(ToHImageList(m_hImageList), __uuidof(IImageList2), imageList.GetAddress())))
		{
			return imageList;
		}
		return nullptr;
	}

	bool ImageList::Create(int width, int height, int initialCount) noexcept
	{
		const bool result = wxImageList::Create(width, height, g_UseMask, initialCount);
		OnCreate(width, height, g_UseMask, initialCount);
		return result;
	}
	bool ImageList::Create(const Size& size, int initialCount) noexcept
	{
		return Create(size.GetWidth(), size.GetHeight(), initialCount);
	}
	bool ImageList::Clear() noexcept
	{
		return wxImageList::RemoveAll();
	}
	bool ImageList::RemoveAll() noexcept
	{
		return wxImageList::RemoveAll();
	}
	bool ImageList::Remove(int index) noexcept
	{
		if (index > 0)
		{
			return wxImageList::Remove(index);
		}
		return false;
	}

	int ImageList::Add(const Bitmap& bitmap)
	{
		return wxImageList::Add(bitmap.ToWxBitmap(), wxNullBitmap);
	}
	int ImageList::Add(const Icon& icon)
	{
		return wxImageList::Add(icon.ToWxIcon());
	}
	int ImageList::Add(const Image& image)
	{
		return Add(image.ToBitmap());
	}

	bool ImageList::Replace(int index, const Bitmap& bitmap)
	{
		return wxImageList::Replace(index, bitmap.ToWxBitmap(), wxNullBitmap);
	}
	bool ImageList::Replace(int index, const Icon& icon)
	{
		return wxImageList::Replace(index, icon.ToWxIcon());
	}
	bool ImageList::Replace(int index, const Image& image)
	{
		return wxImageList::Replace(index, image.ToBitmap().ToWxBitmap());
	}

	Bitmap ImageList::GetBitmap(int index) const
	{
		return Icon(wxImageList::GetIcon(index)).ToBitmap();
	}
	Image ImageList::GetImage(int index) const
	{
		return GetBitmap(index).ToImage();
	}
	Icon ImageList::GetIcon(int index) const
	{
		return wxImageList::GetIcon(index);
	}

	Color ImageList::GetBackgroundColor() const noexcept
	{
		return Color::FromCOLORREF(::ImageList_GetBkColor(ToHImageList(m_hImageList)));
	}
	void ImageList::SetBackgroundColor(const Color& color) noexcept
	{
		::ImageList_SetBkColor(ToHImageList(m_hImageList), color ? color.GetCOLORREF() : CLR_NONE);
	}
	bool ImageList::SetOverlayImage(int index, int overlayIndex) noexcept
	{
		return ::ImageList_SetOverlayImage(ToHImageList(m_hImageList), index, overlayIndex);
	}

	ImageList& ImageList::operator=(ImageList&& other) noexcept
	{
		if (m_hImageList)
		{
			::ImageList_Destroy(ToHImageList(m_hImageList));
			m_hImageList = nullptr;
		}

		m_hImageList = Utility::ExchangeResetAndReturn(other.m_hImageList, nullptr);
		m_size = Utility::ExchangeResetAndReturn(other.m_size, Size::UnspecifiedSize());
		m_Flags = Utility::ExchangeResetAndReturn(other.m_Flags, 0);

		return *this;
	}
}
