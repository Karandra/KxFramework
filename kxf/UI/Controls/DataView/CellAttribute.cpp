#include "stdafx.h"
#include "CellAttribute.h"

namespace kxf::UI::DataView
{
	GDIFont CellAttribute::GetEffectiveFont(const GDIFont& baseFont) const
	{
		if (!m_FontOptions.IsDefault())
		{
			GDIFont font = baseFont;
			if (m_FontOptions.ContainsOption(CellFontOption::Bold))
			{
				font.SetWeight(FontWeight::Bold);
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Italic))
			{
				font.AddStyle(FontStyle::Italic);
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Underlined))
			{
				font.AddStyle(FontStyle::Underline);
			}
			if (m_FontOptions.ContainsOption(CellFontOption::Strikethrough))
			{
				font.AddStyle(FontStyle::Strikethrough);
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
