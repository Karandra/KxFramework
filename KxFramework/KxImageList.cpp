#include "KxStdAfx.h"
#include "KxFramework/KxImageList.h"
#include "Kx/Utility/Common.h"
#include <Shlobj.h>
#include <commoncontrols.h>

namespace
{
	constexpr bool g_UseMask = false;

	uint32_t MapDrawMode(KxImageListDrawMode drawMode)
	{
		using namespace KxEnumClassOperations;

		uint32_t nativeDrawMode = ILD_IMAGE;
		if (drawMode & KxImageListDrawMode::Normal)
		{
			nativeDrawMode |= ILD_NORMAL;
		}
		if (drawMode & KxImageListDrawMode::Transparent)
		{
			nativeDrawMode |= ILD_TRANSPARENT;
		}
		if (drawMode & KxImageListDrawMode::Selected)
		{
			nativeDrawMode |= ILD_SELECTED;
		}
		if (drawMode & KxImageListDrawMode::Focused)
		{
			nativeDrawMode |= ILD_FOCUS;
		}

		return nativeDrawMode;
	}
	HIMAGELIST ToHImageList(WXHIMAGELIST handle)
	{
		return reinterpret_cast<HIMAGELIST>(handle);
	}
}

wxIMPLEMENT_DYNAMIC_CLASS(KxImageList, wxImageList);

void KxImageList::OnCreate(int width, int height, bool mask, int initialCount)
{
	// Replicate wxImageList flags algorithm
	m_Flags = ILC_COLOR32;

	// Common Controls before v6 always uses masks as it doesn't support alpha
	if (mask || wxApp::GetComCtl32Version() < 600)
	{
		m_Flags |= ILC_MASK;
	}
}
bool KxImageList::DoDraw(wxDC& dc, int index, const wxRect& rect, DrawMode drawMode, int overlayIndex)
{
	wxSize size = rect.GetSize();
	size.SetDefaults(wxSize(0, 0));

	uint32_t nativeDrawMode = MapDrawMode(drawMode);
	if (overlayIndex > 0)
	{
		nativeDrawMode |= INDEXTOOVERLAYMASK(overlayIndex);
	}

	return ::ImageList_DrawEx(ToHImageList(m_hImageList), index, dc.GetHDC(), rect.GetX(), rect.GetY(), size.GetWidth(), size.GetHeight(), CLR_NONE, CLR_NONE, nativeDrawMode);
}

KxImageList::KxImageList()
{
	OnCreate(wxDefaultCoord, wxDefaultCoord, g_UseMask, -1);
}
KxImageList::KxImageList(int width, int height, int initialCount)
	:wxImageList(width, height, g_UseMask, initialCount)
{
	OnCreate(width, height, g_UseMask, initialCount);
}
KxImageList::KxImageList(const wxSize& size, int initialCount)
	:KxImageList(size.GetWidth(), size.GetWidth(), initialCount)
{
}
KxImageList::KxImageList(KxImageList&& other)
{
	*this = std::move(other);
}

bool KxImageList::IsOk() const
{
	return m_hImageList != nullptr;
}
bool KxImageList::HasMask() const
{
	return m_Flags & ILC_MASK;
}
KxFramework::COMPtr<IImageList2> KxImageList::QueryInterface() const
{
	KxFramework::COMPtr<IImageList2> imageList;
	if (SUCCEEDED(::HIMAGELIST_QueryInterface(ToHImageList(m_hImageList), __uuidof(IImageList2), imageList.GetAddress())))
	{
		return imageList;
	}
	return nullptr;
}

bool KxImageList::Create(int width, int height, int initialCount)
{
	const bool result = wxImageList::Create(width, height, g_UseMask, initialCount);
	OnCreate(width, height, g_UseMask, initialCount);
	return result;
}
bool KxImageList::Create(const wxSize& size, int initialCount)
{
	return Create(size.GetWidth(), size.GetHeight(), initialCount);
}
bool KxImageList::Clear()
{
	return wxImageList::RemoveAll();
}
bool KxImageList::RemoveAll()
{
	return wxImageList::RemoveAll();
}
bool KxImageList::Remove(int index)
{
	if (index > 0)
	{
		return wxImageList::Remove(index);
	}
	return false;
}

int KxImageList::Add(const wxBitmap& bitmap)
{
	return wxImageList::Add(bitmap, wxNullBitmap);
}
int KxImageList::Add(const wxIcon& icon)
{
	return wxImageList::Add(icon);
}
int KxImageList::Add(const wxImage& image)
{
	return Add(wxBitmap(image, 32));
}

bool KxImageList::Replace(int index, const wxBitmap& bitmap)
{
	return wxImageList::Replace(index, bitmap, wxNullBitmap);
}
bool KxImageList::Replace(int index, const wxIcon& icon)
{
	return wxImageList::Replace(index, icon);
}
bool KxImageList::Replace(int index, const wxImage& image)
{
	return wxImageList::Replace(index, wxBitmap(image, 32));
}

wxBitmap KxImageList::GetBitmap(int index) const
{
	return wxBitmap(GetIcon(index), wxBitmapTransparency::wxBitmapTransparency_Auto);
}
wxImage KxImageList::GetImage(int index) const
{
	wxBitmap bitmap = GetBitmap(index);
	wxImage image = bitmap.ConvertToImage();
	if (!image.HasAlpha() && bitmap.HasAlpha())
	{
		image.InitAlpha();
	}
	return image;
}
wxIcon KxImageList::GetIcon(int index) const
{
	return wxImageList::GetIcon(index);
}

KxColor KxImageList::GetBackgroundColor() const
{
	return KxColor().SetCOLORREF(::ImageList_GetBkColor(ToHImageList(m_hImageList)));
}
void KxImageList::SetBackgroundColor(const KxColor& color)
{
	::ImageList_SetBkColor(ToHImageList(m_hImageList), color ? color.GetCOLORREF() : CLR_NONE);
}
bool KxImageList::SetOverlayImage(int index, int overlayIndex)
{
	return ::ImageList_SetOverlayImage(ToHImageList(m_hImageList), index, overlayIndex);
}

KxImageList& KxImageList::operator=(KxImageList&& other)
{
	if (m_hImageList)
	{
		::ImageList_Destroy(ToHImageList(m_hImageList));
		m_hImageList = nullptr;
	}

	using namespace KxFramework;
	m_hImageList = Utility::ExchangeResetAndReturn(other.m_hImageList, nullptr);
	m_size = Utility::ExchangeResetAndReturn(other.m_size, wxDefaultSize);
	m_Flags = Utility::ExchangeResetAndReturn(other.m_Flags, 0);

	return *this;
}
