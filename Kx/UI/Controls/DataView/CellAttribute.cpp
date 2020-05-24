#include "stdafx.h"
#include "CellAttribute.h"

namespace kxf::UI::DataView
{
	wxFont CellAttribute::GetEffectiveFont(const wxFont& baseFont) const
	{
		if (!m_FontOptions.IsDefault())
		{
			wxFont font(baseFont);
			if (m_FontOptions.ContainsOption(CellFontOption::Bold))
			{
				font.MakeBold();
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Italic))
			{
				font.MakeItalic();
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Underlined))
			{
				font.MakeUnderlined();
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Strikethrough))
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
