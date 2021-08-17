#include "KxfPCH.h"
#include "Slider.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/System.h" 

#include <CommCtrl.h>
#include "kxf/System/UndefWindows.h" 

namespace kxf::WXUI
{
	bool Slider::Create(wxWindow* parent,
						const String& label,
						const Point& pos,
						const Size& size
	)
	{
		if (wxSlider::Create(parent, wxID_NONE, 0, 0, 100, pos, size, wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_SELRANGE, wxDefaultValidator))
		{
			return true;
		}
		return false;
	}
}
