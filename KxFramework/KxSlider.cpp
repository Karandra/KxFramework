#include "KxStdAfx.h"
#include "KxFramework/KxSlider.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/System.h" 

#include <CommCtrl.h>
#include "Kx/System/UndefWindows.h" 

using namespace KxFramework;

wxIMPLEMENT_DYNAMIC_CLASS(KxSlider, wxSlider);

bool KxSlider::Create(wxWindow* parent,
					  wxWindowID id,
					  int value,
					  int minValue,
					  int maxValue,
					  long style,
					  const wxValidator& validator
)
{
	Utility::ModFlagRef(style, KxSL_VERTICAL, style & (KxSL_LEFT|KxSL_RIGHT));
	Utility::ModFlagRef(style, KxSL_HORIZONTAL, style & (KxSL_TOP|KxSL_BOTH));

	// Remove 'KxSL_NO_THUMB' from default wx flags. Just in case.
	if (wxSlider::Create(parent, id, value, minValue, maxValue, wxDefaultPosition, wxDefaultSize, Utility::ModFlag(style, KxSL_NO_THUMB, false), validator))
	{
		if (style & KxSL_BOTH)
		{
			Utility::ModWindowStyle(GetHandle(), GWL_STYLE, TBS_BOTH, true);
		}
		if (style & KxSL_NO_THUMB)
		{
			Utility::ModWindowStyle(GetHandle(), GWL_STYLE, TBS_NOTHUMB, true);
		}
		return true;
	}
	return false;
}
