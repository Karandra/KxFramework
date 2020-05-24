#pragma once
#include "Kx/Common.hpp"
#include <wx/gdicmn.h>
#include "Kx/System/UndefWindows.h"
struct tagRECT;

namespace kxf::Utility
{
	void FromWindowsRect(const tagRECT& winRect, Rect& rect) noexcept;
	Rect FromWindowsRect(const tagRECT& winRect) noexcept;

	void ToWindowsRect(const Rect& rect, tagRECT& winRect) noexcept;
	tagRECT ToWindowsRect(const Rect& rect) noexcept;
}
