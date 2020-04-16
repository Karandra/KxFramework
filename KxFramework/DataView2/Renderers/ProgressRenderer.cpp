#include "stdafx.h"
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
			renderEngine.DrawProgressBar(GetBarRect(), cellState, m_Value.GetPosition(), m_Value.GetRange(), m_Value.GetState());
		}
		if (m_Value.HasText())
		{
			renderEngine.DrawText(cellRect, cellState, m_Value.GetText());
		}
	}
	wxSize ProgressRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			RenderEngine renderEngine = GetRenderEngine();
			wxSize size = renderEngine.GetTextExtent(m_Value.GetText());

			// If we need to draw a progress bar, then add a small margin to prevent clipping
			if (m_Value.HasPosition())
			{
				size += renderEngine.FromDIP(8, 0);
			}
			return size;
		}
		else
		{
			return wxSize(wxDefaultCoord, GetBarRect().GetHeight());
		}
	}
	wxRect ProgressRenderer::GetBarRect() const
	{
		RenderEngine renderEngine = GetRenderEngine();
		const wxRect paintRect = GetPaintRect();

		wxRect barRect = paintRect;
		auto SetHeight = [&](int desiredHeight, int margin = 0)
		{
			if (desiredHeight < barRect.GetHeight())
			{
				barRect.Inflate(0, barRect.GetHeight() - desiredHeight);
			}
			barRect.Deflate(0, margin);
		};

		const int margin = renderEngine.FromDIPY(4);
		switch (m_Value.GetHeight())
		{
			case ProgressValue::Height::Auto:
			{
				SetHeight(GetView()->GetCharHeight() + renderEngine.FromDIPY(2), margin);
				break;
			}
			case ProgressValue::Height::Fit:
			{
				barRect.Deflate(0, margin);
				break;
			}
			default:
			{
				SetHeight(m_Value.GetHeight<int>(), margin);
				break;
			}
		};

		if (barRect.GetHeight() >= paintRect.GetHeight())
		{
			barRect.Deflate(0, margin);
		}
		return barRect;
	}
}
