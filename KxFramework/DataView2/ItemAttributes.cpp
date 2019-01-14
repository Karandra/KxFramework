#include "KxStdAfx.h"
#include "ItemAttributes.h"

namespace Kx::DataView2
{
	void ItemAttributes::Reset()
	{
		*this = ItemAttributes();
	}
	wxFont ItemAttributes::GetEffectiveFont(const wxFont& baseFont) const
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
			if (HasFontSize())
			{
				font.SetPointSize(m_FontSize);
			}
			return font;
		}
		return baseFont;
	}
}
