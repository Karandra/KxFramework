#pragma once
#include "kxf/Common.hpp"
#include <wx/gdicmn.h>
#include "kxf/System/UndefWindows.h"
struct tagRECT;

namespace kxf::Utility
{
	KX_API void FromWindowsRect(const tagRECT& winRect, Rect& rect) noexcept;
	KX_API Rect FromWindowsRect(const tagRECT& winRect) noexcept;

	KX_API void ToWindowsRect(const Rect& rect, tagRECT& winRect) noexcept;
	KX_API tagRECT ToWindowsRect(const Rect& rect) noexcept;
}
