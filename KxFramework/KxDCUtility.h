#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxColor.h"

namespace KxDCUtility
{
	KxColor GetAverageColor(const wxDC& dc, const wxRect& rect);
}
