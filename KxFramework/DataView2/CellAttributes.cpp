#include "KxStdAfx.h"
#include "CellAttributes.h"

namespace KxDataView2
{
	void CellAttributes::Reset()
	{
		*this = CellAttributes();
	}
	wxFont CellAttributes::GetEffectiveFont(const wxFont& baseFont) const
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
