#pragma once
#include "../Common.h"

namespace kxf::WXUI::DataView
{
	enum class DNDOpType: uint32_t
	{
		None = 0,
		Drag = 1 << 0,
		Drop = 1 << 1,
	};
}

namespace kxf
{
	KxFlagSet_Declare(WXUI::DataView::DNDOpType);
}
