#include "stdafx.h"
#include "TextRenderer.h"

namespace kxf::UI::DataView
{
	bool TextValue::FromAny(const Any& value)
	{
		return value.GetAs(m_Text) || value.GetAs(*this);
	}
}

namespace kxf::UI::DataView
{
	bool TextRenderer::SetValue(const Any& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}

	void TextRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		if (m_Value.HasText())
		{
			GetRenderEngine().DrawText(cellRect, cellState, m_Value.GetText());
		}
	}
	Size TextRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			return GetRenderEngine().GetTextExtent(m_Value.GetText());
		}
		return Size(0, 0);
	}
}
