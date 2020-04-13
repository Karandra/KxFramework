#pragma once
#include "Kx/Common.hpp"
#include <windef.h>
#include <wx/gdicmn.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework::Utility
{
	inline void FromWindowsRect(const RECT& winRect, wxRect& rect) noexcept
	{
		rect.y = winRect.top;
		rect.x = winRect.left;
		rect.width = winRect.right - winRect.left;
		rect.height = winRect.bottom - winRect.top;
	}
	inline wxRect FromWindowsRect(const RECT& winRect) noexcept
	{
		wxRect rect;
		FromWindowsRect(winRect, rect);
		return rect;
	}

	inline void ToWindowsRect(const wxRect& rect, RECT& winRect) noexcept
	{
		winRect.top = rect.y;
		winRect.left = rect.x;
		winRect.right = rect.x + rect.width;
		winRect.bottom = rect.y + rect.height;
	}
	inline RECT ToWindowsRect(const wxRect& rect) noexcept
	{
		RECT winRect;
		ToWindowsRect(rect, winRect);
		return winRect;
	}
}
