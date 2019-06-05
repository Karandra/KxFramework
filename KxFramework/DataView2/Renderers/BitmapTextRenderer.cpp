#include "KxStdAfx.h"
#include "BitmapTextRenderer.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace KxDataView2
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
		const bool centerTextV = m_Value.IsOptionEnabled(BitmapTextValueOptions::VCenterText);
		const int reservedWidth = m_Value.GetReservedBitmapWidth();

		GetRenderEngine().DrawBitmapWithText(cellRect, cellState, 0, m_Value.GetText(), m_Value.GetBitmap(), centerTextV, reservedWidth);
	}
	wxSize BitmapTextRenderer::GetCellSize() const
	{
		RenderEngine renderEngine = GetRenderEngine();

		wxSize size(0, 0);
		if (m_Value.HasText())
		{
			size += renderEngine.GetTextExtent(m_Value.GetText());
		}
		if (m_Value.HasBitmap())
		{
			const wxBitmap& bitmap = m_Value.GetBitmap();
			size.x += bitmap.GetWidth() + GetRenderEngine().FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.y < bitmap.GetHeight())
			{
				size.y = bitmap.GetHeight();
			}
		}
		else if (int reservedWidth = m_Value.GetReservedBitmapWidth(); reservedWidth > 0)
		{
			size.x += reservedWidth + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
		}
		return size;
	}
}
