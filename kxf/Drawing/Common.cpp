#include "stdafx.h"
#include "Common.h"
#include "Bitmap.h"
#include "Icon.h"
#include "Image.h"

namespace kxf::Drawing
{
	wxIcon ToIcon(const wxImage& image)
	{
		return Image(image).ToIcon().ToWxIcon();
	}
	wxIcon ToIcon(const wxBitmap& bitmap)
	{
		return Bitmap(bitmap).ToIcon().ToWxIcon();
	}

	wxImage ToImage(const wxIcon& icon)
	{
		return Icon(icon).ToImage().ToWxImage();
	}
	wxImage ToImage(const wxBitmap& bitmap)
	{
		return Bitmap(bitmap).ToImage().ToWxImage();
	}

	wxBitmap ToBitmap(const wxIcon& icon)
	{
		return Icon(icon).ToBitmap().ToWxBitmap();
	}
	wxBitmap ToBitmap(const wxImage& image)
	{
		return Image(image).ToBitmap().ToWxBitmap();
	}
}
