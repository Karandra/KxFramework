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
		// Return 'wxDefaultCoord' for width because a progress bar fits any width 
		// unless it has a text string). Unlike most renderers, it doesn't have a "good" width
		// for the content. This makes it grow/ to the whole column, which is pretty much always
		// the desired. Unless it has a text string.

		wxSize size;
		if (m_Value.HasText())
		{
			size += GetRenderEngine().GetTextExtent(m_Value.GetText());
		}

		wxSize barSize;
		switch (m_Height)
		{
			case ProgressHeight::Auto:
			{
				barSize = wxSize(wxDefaultCoord, std::max(GetView()->GetDefaultRowHeight(UniformHeight::Default), GetView()->GetCharHeight() + 2));
				break;
			}
			case ProgressHeight::Fit:
			{
				barSize = wxSize(wxDefaultCoord, GetView()->GetUniformRowHeight() - GetView()->FromDIP(4));
				break;
			}
			default:
			{
				barSize = wxSize(wxDefaultCoord, static_cast<int>(m_Height));
				break;
			}
		};

		if (size != wxSize())
		{
			size.IncTo(barSize);
			return size;
		}
		else
		{
			return barSize;
		}
	}
}
