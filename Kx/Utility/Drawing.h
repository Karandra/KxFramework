#pragma once
#include "Kx/Common.hpp"
#include <wx/gdicmn.h>
#include "Kx/System/UndefWindows.h"
struct tagRECT;

namespace KxFramework::Utility
{
	void FromWindowsRect(const tagRECT& winRect, wxRect& rect) noexcept;
	wxRect FromWindowsRect(const tagRECT& winRect) noexcept;

	void ToWindowsRect(const wxRect& rect, tagRECT& winRect) noexcept;
	tagRECT ToWindowsRect(const wxRect& rect) noexcept;
}
