#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewTextRenderer.h"

bool KxDataViewTextRenderer::SetValue(const wxAny& value)
{
	m_Text.clear();
	return GetValueAsString(value, m_Text);
}

void KxDataViewTextRenderer::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	DoDrawText(cellRect, cellState, m_Text);
}
wxSize KxDataViewTextRenderer::GetCellSize() const
{
	if (!m_Text.IsEmpty())
	{
		return DoGetTextExtent(m_Text);
	}
	return wxSize(0, KxDataViewRenderer::GetCellSize().GetHeight());
}
