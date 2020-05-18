#include "stdafx.h"
#include "Drawing.h"
#include <Windows.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework::Utility
{
	void FromWindowsRect(const tagRECT& winRect, wxRect& rect) noexcept
	{
		rect.y = winRect.top;
		rect.x = winRect.left;
		rect.width = winRect.right - winRect.left;
		rect.height = winRect.bottom - winRect.top;
	}
	wxRect FromWindowsRect(const tagRECT& winRect) noexcept
	{
		wxRect rect;
		FromWindowsRect(winRect, rect);
		return rect;
	}

	void ToWindowsRect(const wxRect& rect, tagRECT& winRect) noexcept
	{
		winRect.top = rect.y;
		winRect.left = rect.x;
		winRect.right = rect.x + rect.width;
		winRect.bottom = rect.y + rect.height;
	}
	tagRECT ToWindowsRect(const wxRect& rect) noexcept
	{
		tagRECT winRect;
		ToWindowsRect(rect, winRect);
		return winRect;
	}
}
