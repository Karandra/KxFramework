#include "stdafx.h"
#include "GDI.h"
#include "../Bitmap.h"
#include <wx/pen.h>
#include <wx/brush.h>
#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::Drawing::Private
{
	Bitmap BitmapFromMemoryLocation(const void* data)
	{
		const BITMAPINFO& bitmapInfo = *static_cast<const BITMAPINFO*>(data);
		const BITMAPINFOHEADER& bitmapHeader = bitmapInfo.bmiHeader;

		// We don't support other color depths here
		const ColorDepth colorDepth = bitmapHeader.biBitCount;
		if (colorDepth == ColorDepthDB::BPP24 || colorDepth == ColorDepthDB::BPP32)
		{
			// TODO: Check the correctness of this pointer
			const auto bitmapData = reinterpret_cast<const uint8_t*>(data) + sizeof(BITMAPINFO);

			Bitmap bitmap;
			bitmap.AttachHandle(::CreateBitmap(bitmapHeader.biWidth, bitmapHeader.biHeight, bitmapHeader.biPlanes, bitmapHeader.biBitCount, bitmapData));
			return bitmap;
		}
		return {};
	}

	std::optional<int> MapHatchStyle(wxHatchStyle style) noexcept
	{
		switch (style)
		{
			case wxHATCHSTYLE_BDIAGONAL:
			{
				return HS_BDIAGONAL;
			}
			case wxHATCHSTYLE_CROSSDIAG:
			{
				return HS_DIAGCROSS;
			}
			case wxHATCHSTYLE_FDIAGONAL:
			{
				return HS_FDIAGONAL;
			}
			case wxHATCHSTYLE_CROSS:
			{
				return HS_CROSS;
			}
			case wxHATCHSTYLE_HORIZONTAL:
			{
				return HS_HORIZONTAL;
			}
			case wxHATCHSTYLE_VERTICAL:
			{
				return HS_VERTICAL;
			}
		};
		return {};
	}
	wxHatchStyle MapHatchStyle(int style) noexcept
	{
		switch (style)
		{
			case HS_BDIAGONAL:
			{
				return wxHATCHSTYLE_BDIAGONAL;
			}
			case HS_DIAGCROSS:
			{
				return wxHATCHSTYLE_CROSSDIAG;
			}
			case HS_FDIAGONAL:
			{
				return wxHATCHSTYLE_FDIAGONAL;
			}
			case HS_CROSS:
			{
				return wxHATCHSTYLE_CROSS;
			}
			case HS_HORIZONTAL:
			{
				return wxHATCHSTYLE_HORIZONTAL;
			}
			case HS_VERTICAL:
			{
				return wxHATCHSTYLE_VERTICAL;
			}
		};
		return {};
	}

	wxPenStyle MapPenStyle(int style) noexcept
	{
		switch (style)
		{
			case PS_DASH:
			{
				return wxPENSTYLE_SHORT_DASH;
			}
			case PS_DOT:
			{
				return wxPENSTYLE_DOT;
			}
			case PS_DASHDOT:
			case PS_DASHDOTDOT:
			{
				return wxPENSTYLE_DOT_DASH;
			}
			case PS_SOLID:
			{
				return wxPENSTYLE_SOLID;
			}
			case PS_USERSTYLE:
			{
				return wxPENSTYLE_USER_DASH;
			}
			case PS_NULL:
			{
				return wxPENSTYLE_TRANSPARENT;
			}
		};
		return wxPENSTYLE_INVALID;
	}
	wxPenJoin MapPenJoin(int join) noexcept
	{
		switch (join)
		{
			case PS_JOIN_BEVEL:
			{
				return wxJOIN_BEVEL;
			}
			case PS_JOIN_MITER:
			{
				return wxJOIN_MITER;
			}
			case PS_JOIN_ROUND:
			{
				return wxJOIN_ROUND;
			}
		};
		return wxJOIN_INVALID;
	}
}
