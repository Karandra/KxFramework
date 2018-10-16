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
		wxFont font(baseFont);
		if (IsBold())
		{
			font.MakeBold();
		}
		if (IsItalic())
		{
			font.MakeItalic();
		}
		if (IsUnderlined())
		{
			font.MakeUnderlined();
		}
		if (IsStrikethrough())
		{
			font.MakeStrikethrough();
		}
		if (HasFontFace())
		{
			font.SetFaceName(m_FontFace);
		}
		if (HasFontFamily())
		{
			font.SetFamily(m_FontFamily);
		}
		if (HasFontSize())
		{
			font.SetPointSize(m_FontSize);
		}
		if (HasFontPixelSize())
		{
			font.SetPixelSize(m_FontPixelSize);
		}

		return font;
	}
	return baseFont;
}
