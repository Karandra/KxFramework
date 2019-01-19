#include "KxStdAfx.h"
#include "BitmapTextRenderer.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace Kx::DataView2
{
	bool BitmapTextRenderer::SetValue(const wxAny& value)
	{
		m_Value = BitmapTextValue();
		if (value.GetAs<BitmapTextValue>(&m_Value))
		{
			return true;
		}
		if (BitmapRenderer::GetValueAsBitmap(value, m_Value.GetBitmap()))
		{
			return true;
		}
		if (TextRenderer::GetValueAsString(value, m_Value.GetText()))
		{
			return true;
		}
		return false;
	}

	void BitmapTextRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		GetRenderEngine().DrawBitmapWithText(cellRect, cellState, 0, m_Value.GetText(), m_Value.GetBitmap(), m_Value.ShouldVCenterText());
	}
	wxSize BitmapTextRenderer::GetCellSize() const
	{
		wxSize size(0, 0);
		if (m_Value.HasText())
		{
			size += GetRenderEngine().GetTextExtent(m_Value.GetText());
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
