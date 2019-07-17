#include "KxStdAfx.h"
#include "ProgressRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"

namespace KxDataView2
{
	bool ProgressValueBase::FromAny(const wxAny& value)
	{
		return value.GetAs(&m_Position) || value.GetAs(this);
	}
	bool ProgressValue::FromAny(const wxAny& value)
	{
		return TextValue::FromAny(value) || ProgressValueBase::FromAny(value) || value.GetAs(this);
	}
}

namespace KxDataView2
{
	bool ProgressRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}

	void ProgressRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		RenderEngine renderEngine = GetRenderEngine();
		if (m_Value.HasPosition())
		{
			renderEngine.DrawProgressBar(cellRect, cellState, m_Value.GetPosition(), m_Value.GetRange(), m_Value.GetState());
		}
		if (m_Value.HasText())
		{
			wxRect textRect = wxRect(cellRect).Deflate(renderEngine.FromDIP(4, 4));
			textRect = renderEngine.CenterTextInside(textRect, renderEngine.GetTextExtent(m_Value.GetText()));

			renderEngine.DrawText(textRect, cellState, m_Value.GetText());
		}
	}
	wxSize ProgressRenderer::GetCellSize() const
	{
		// Return 'wxDefaultCoord' for width because a progress bar fits any width 
		// unless it has a text string). Unlike most renderers, it doesn't have a "good" width
		// for the content. This makes it grow/ to the whole column, which is pretty much always
		// the desired.

		switch (m_Value.GetHeight())
		{
			case ProgressValue::Height::Auto:
			{
				return wxSize(wxDefaultCoord, std::max(GetView()->GetDefaultRowHeight(UniformHeight::Default), GetView()->GetCharHeight() + 2));
			}
			case ProgressValue::Height::Fit:
			{
				RenderEngine renderEngine = GetRenderEngine();
				return wxSize(wxDefaultCoord, GetView()->GetUniformRowHeight() - renderEngine.FromDIPY(4));
			}
		};
		return wxSize(wxDefaultCoord, m_Value.GetHeight<int>());
	}
}
