#include "KxStdAfx.h"
#include "TextRenderer.h"

namespace Kx::DataView2
{
	bool TextRenderer::SetValue(const wxAny& value)
	{
		m_Text.clear();
		return GetValueAsString(value, m_Text);
	}

	void TextRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		DoDrawText(cellRect, cellState, m_Text);
	}
	wxSize TextRenderer::GetCellSize() const
	{
		if (!m_Text.IsEmpty())
		{
			return DoGetTextExtent(m_Text);
		}
		return wxSize(0, Renderer::GetCellSize().GetHeight());
	}
}
