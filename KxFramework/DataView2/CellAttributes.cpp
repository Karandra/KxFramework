#include "stdafx.h"
#include "CellAttributes.h"

namespace KxDataView2
{
	wxFont CellAttributes::GetEffectiveFont(const wxFont& baseFont) const
	{
		if (!m_FontOptions.IsDefault())
		{
			wxFont font(baseFont);
			if (m_FontOptions.IsEnabled(CellFontOption::Bold))
			{
				font.MakeBold();
			}
			if (m_FontOptions.IsEnabled(CellFontOption::Italic))
			{
				font.MakeItalic();
			}
			if (m_FontOptions.IsEnabled(CellFontOption::Underlined))
			{
				font.MakeUnderlined();
			}
			if (m_FontOptions.IsEnabled(CellFontOption::Strikethrough))
			{
				font.MakeStrikethrough();
			}
			if (m_FontOptions.HasFontFace())
			{
				font.SetFaceName(m_FontOptions.GetFontFace());
			}
			if (m_FontOptions.HasFontSize())
			{
				font.SetPointSize(m_FontOptions.GetFontSize());
			}
			return font;
		}
		return baseFont;
	}
}
