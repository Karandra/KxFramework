#include "stdafx.h"
#include "Slider.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/System.h" 

#include <CommCtrl.h>
#include "Kx/System/UndefWindows.h" 

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Slider, wxSlider);

	bool Slider::Create(wxWindow* parent,
						wxWindowID id,
						int value,
						int minValue,
						int maxValue,
						SliderStyle style,
						const wxValidator& validator
	)
	{
		Utility::ModFlagRef(style, SliderStyle::Vertical, style & (SliderStyle::Left|SliderStyle::Right));
		Utility::ModFlagRef(style, SliderStyle::Horizontal, style & (SliderStyle::Top|SliderStyle::Both));

		// Remove 'SliderStyle::NoThumb' from default wx flags. Just in case.
		if (wxSlider::Create(parent, id, value, minValue, maxValue, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), ToInt(Utility::ModFlag(style, SliderStyle::NoThumb, false)), validator))
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
