#include "KxStdAfx.h"
#include "BitmapTextToggleRenderer.h"
#include "BitmapRenderer.h"
#include "TextRenderer.h"
#include "ToggleRenderer.h"
#include "KxFramework/DataView2/View.h"

namespace Kx::DataView2
{
	wxAny BitmapTextToggleRenderer::OnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent)
	{
		ToggleState state = m_Value.GetState();
		if (DoOnActivateCell(GetRenderEngine().GetToggleSize(), state, mouseEvent))
		{
			return state;
		}
		return {};
	}

	bool BitmapTextToggleRenderer::SetValue(const wxAny& value)
	{
		m_Value = BitmapTextToggleValue();
		if (!value.GetAs(&m_Value))
		{
			if (ToggleRenderer::GetValueAsToggleState(value, m_Value.GetState()))
			{
				if (!m_Value.HasType())
				{
					m_Value.SetType(GetDefaultToggleType());
				}
				return true;
			}
			if (TextRenderer::GetValueAsString(value, m_Value.GetText()))
			{
				return true;
			}
			if (BitmapRenderer::GetValueAsBitmap(value, m_Value.GetBitmap()))
			{
				return true;
			}
		}
		return false;
	}
	void BitmapTextToggleRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		int offsetX = 0;
		int offsetFromToggle = 0;

		if (m_Value.HasType())
		{
			wxRect toggleRect(cellRect.GetPosition(), GetRenderEngine().GetToggleSize());
			toggleRect.SetHeight(cellRect.GetHeight());

			offsetX += GetRenderEngine().DrawToggle(GetGraphicsDC(), toggleRect, cellState, m_Value.GetState(), m_Value.GetType()).GetWidth();
			offsetFromToggle = GetRenderEngine().FromDIPX(2);
		}
		if (m_Value.HasBitmap() || m_Value.HasText())
		{
			wxRect rect = cellRect;
			rect.x += offsetFromToggle;
			rect.width -= offsetFromToggle;

			GetRenderEngine().DrawBitmapWithText(rect, cellState, offsetX, m_Value.GetText(), m_Value.GetBitmap(), m_Value.ShouldVCenterText());
		}
	}
	wxSize BitmapTextToggleRenderer::GetCellSize() const
	{
		wxSize size(0, 0);
		if (m_Value.HasType())
		{
			wxSize toggleSize = GetRenderEngine().GetToggleSize();
			size.x += toggleSize.x + GetRenderEngine().FromDIPX(2);
			if (size.y < toggleSize.y)
			{
				size.y = toggleSize.y;
			}
		}
		if (m_Value.HasText())
		{
			wxSize textExtent = GetRenderEngine().GetTextExtent(m_Value.GetText());
			size.x += textExtent.x;
			if (size.y < textExtent.y)
			{
				size.y = textExtent.y;
			}
		}
		if (m_Value.HasBitmap())
		{
			const wxBitmap& bitmap = m_Value.GetBitmap();
			size.x += bitmap.GetWidth() + GetRenderEngine().FromDIPX(1);
			if (size.y < bitmap.GetHeight())
			{
				size.y = bitmap.GetHeight();
			}
		}
		return size;
	}
}
