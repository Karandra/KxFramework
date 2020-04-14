#pragma once
#include "Kx/Common.hpp"
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework
{
	enum class GraphicsRendererType
	{
		None = -1,
		Direct2D,
		GDIPlus,
		Sciter,
		Cairo
	};
}

namespace KxFramework::Drawing
{
	constexpr int InvalidImageIndex = -1;
}

namespace KxFramework::Drawing
{
	wxIcon ToIcon(const wxImage& image);
	wxIcon ToIcon(const wxBitmap& bitmap);

	wxImage ToImage(const wxIcon& icon);
	wxImage ToImage(const wxBitmap& bitmap);

	wxBitmap ToBitmap(const wxIcon& icon);
	wxBitmap ToBitmap(const wxImage& image);
}
