#pragma once
#include "kxf/Common.hpp"
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	class Bitmap;
	class Cursor;
	class Image;
	class Icon;

	enum class GraphicsRendererType
	{
		None = -1,
		Direct2D,
		GDIPlus,
		Sciter,
		Cairo
	};
	enum class ImageFormat
	{
		None = wxBITMAP_TYPE_INVALID,
		Any = wxBITMAP_TYPE_ANY,

		Bitmap = wxBITMAP_TYPE_BMP,
		Icon = wxBITMAP_TYPE_ICO,
		Cursor = wxBITMAP_TYPE_CUR,
		AnimatedCursor = wxBITMAP_TYPE_ANI,

		PNG = wxBITMAP_TYPE_PNG,
		GIF = wxBITMAP_TYPE_GIF,
		JPEG = wxBITMAP_TYPE_JPEG,
		TGA = wxBITMAP_TYPE_TGA,
		PNM = wxBITMAP_TYPE_PNM,
		IFF = wxBITMAP_TYPE_IFF,
		PCX = wxBITMAP_TYPE_PCX,
		XBM = wxBITMAP_TYPE_XBM,
		XPM = wxBITMAP_TYPE_XPM,
		TIF = wxBITMAP_TYPE_TIF,
		TIFF = wxBITMAP_TYPE_TIFF,
		PICT = wxBITMAP_TYPE_PICT,
	};
}

namespace kxf::Drawing
{
	constexpr int InvalidImageIndex = -1;
}

namespace kxf::Drawing
{
	wxIcon ToIcon(const wxImage& image);
	wxIcon ToIcon(const wxBitmap& bitmap);

	wxImage ToImage(const wxIcon& icon);
	wxImage ToImage(const wxBitmap& bitmap);

	wxBitmap ToBitmap(const wxIcon& icon);
	wxBitmap ToBitmap(const wxImage& image);
}
