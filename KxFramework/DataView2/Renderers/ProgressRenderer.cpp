#include "KxStdAfx.h"
#include "ProgressRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"

namespace KxDataView2
{
	bool ProgressRenderer::SetValue(const wxAny& value)
	{
		m_Value = ProgressValue();
		if (!value.GetAs(&m_Value))
		{
			int position = -1;
			if (value.GetAs(&position))
			{
				m_Value.SetPosition(position);
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	void ProgressRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		if (m_Value.HasPosition())
		{
			GetRenderEngine().DrawProgressBar(cellRect, cellState, m_Value.GetPosition(), m_Value.GetRange(), m_Value.GetState());
		}
		if (m_Value.HasText())
		{
			GetRenderEngine().DrawText(cellRect, cellState, m_Value.GetText());
		}
	}
	wxSize ProgressRenderer::GetCellSize() const
	{
		// Return 'wxDefaultCoord' because a progress bar fits any width. Unlike most renderers,
		// it doesn't have a "good" width for the content. This makes it grow to the whole column,
		// which is pretty much always the desired.

		switch (m_Height)
		{
			case ProgressHeight::Auto:
			{
				return wxSize(wxDefaultCoord, std::max(GetView()->GetDefaultRowHeight(UniformHeight::Default), GetView()->GetCharHeight() + 2));
			}
			case ProgressHeight::Fit:
			{
				return wxSize(wxDefaultCoord, GetView()->GetUniformRowHeight() - GetView()->FromDIP(4));
			}
		};
		return wxSize(0, static_cast<int>(m_Height));
	}
}
