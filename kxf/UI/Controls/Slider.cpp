#include "stdafx.h"
#include "Slider.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/System.h" 

#include <CommCtrl.h>
#include "kxf/System/UndefWindows.h" 

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Slider, wxSlider);

	bool Slider::Create(wxWindow* parent,
						wxWindowID id,
						int value,
						int minValue,
						int maxValue,
						FlagSet<SliderStyle> style,
						const wxValidator& validator
	)
	{
		style.Mod(SliderStyle::Vertical, style & (SliderStyle::Left|SliderStyle::Right));
		style.Mod(SliderStyle::Horizontal, style & (SliderStyle::Top|SliderStyle::Both));

		// Remove 'SliderStyle::NoThumb' from default wx flags. Just in case.
		if (wxSlider::Create(parent, id, value, minValue, maxValue, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style.Remove(SliderStyle::NoThumb).ToInt(), validator))
		{
			if (style & SliderStyle::Both)
			{
				Utility::ModWindowStyle(GetHandle(), GWL_STYLE, TBS_BOTH, true);
			}
			if (style & SliderStyle::NoThumb)
			{
				Utility::ModWindowStyle(GetHandle(), GWL_STYLE, TBS_NOTHUMB, true);
			}
			return true;
		}
		return false;
	}
}
