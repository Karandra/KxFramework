#include "KxStdAfx.h"
#include "KxFramework/KxSlider.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxIncludeWindows.h"

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
	KxUtility::ModFlagRef(style, KxSL_VERTICAL, style & (KxSL_LEFT|KxSL_RIGHT));
	KxUtility::ModFlagRef(style, KxSL_HORIZONTAL, style & (KxSL_TOP|KxSL_BOTH));

	// Remove 'KxSL_NO_THUMB' from default wx flags. Just in case.
	if (wxSlider::Create(parent, id, value, minValue, maxValue, wxDefaultPosition, wxDefaultSize, KxUtility::ModFlag(style, KxSL_NO_THUMB, false), validator))
	{
		if (style & KxSL_BOTH)
		{
			KxUtility::ToggleWindowStyle(GetHandle(), GWL_STYLE, TBS_BOTH, true);
		}
		if (style & KxSL_NO_THUMB)
		{
			KxUtility::ToggleWindowStyle(GetHandle(), GWL_STYLE, TBS_NOTHUMB, true);
		}
		return true;
	}
	return false;
}
