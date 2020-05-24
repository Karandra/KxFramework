#include "stdafx.h"
#include "Common.h"

namespace
{
	constexpr int g_BitmapDepth = 32;
	constexpr wxBitmapTransparency g_BitmapTransparency = wxBitmapTransparency::wxBitmapTransparency_Always;
}

namespace kxf::Drawing
{
	wxIcon ToIcon(const wxImage& image)
	{
		return ToIcon(ToBitmap(image));
	}
	wxIcon ToIcon(const wxBitmap& bitmap)
	{
		wxIcon icon;
		icon.CopyFromBitmap(bitmap);
		return icon;
	}

	wxImage ToImage(const wxIcon& icon)
	{
		return ToImage(ToBitmap(icon));
	}
	wxImage ToImage(const wxBitmap& bitmap)
	{
		return bitmap.ConvertToImage();
	}

	wxBitmap ToBitmap(const wxIcon& icon)
	{
		return wxBitmap(icon, g_BitmapTransparency);
	}
	wxBitmap ToBitmap(const wxImage& image)
	{
		return wxBitmap(image, g_BitmapDepth);
	}
}
