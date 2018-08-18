#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewItemAttributes.h"

void KxDataViewItemAttributes::Reset()
{
	*this = KxDataViewItemAttributes();
}
wxFont KxDataViewItemAttributes::GetEffectiveFont(const wxFont& baseFont) const
{
	if (HasFontAttributes())
	{
		wxFont newFont(baseFont);
		if (IsBold())
		{
			newFont.MakeBold();
		}
		if (IsItalic())
		{
			newFont.MakeItalic();
		}
		if (IsUnderlined())
		{
			newFont.MakeUnderlined();
		}
		if (IsStrikethrough())
		{
			newFont.MakeStrikethrough();
		}
		if (HasFontFace())
		{
			newFont.SetFaceName(m_FontFace);
		}
		if (HasFontFamily())
		{
			newFont.SetFamily(m_FontFamily);
		}

		return newFont;
	}
	return baseFont;
}
