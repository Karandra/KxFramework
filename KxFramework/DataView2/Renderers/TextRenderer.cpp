#include "stdafx.h"
#include "TextRenderer.h"

namespace KxDataView2
{
	bool TextValue::FromAny(const wxAny& value)
	{
		return value.GetAs(&m_Text) || value.GetAs(this);
	}
}

namespace KxDataView2
{
	bool TextRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}

	void TextRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		if (m_Value.HasText())
		{
			GetRenderEngine().DrawText(cellRect, cellState, m_Value.GetText());
		}
	}
	wxSize TextRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			return GetRenderEngine().GetTextExtent(m_Value.GetText());
		}
		return wxSize(0, 0);
	}
}
