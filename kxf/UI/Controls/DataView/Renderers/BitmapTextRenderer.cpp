#include "stdafx.h"
#include "BitmapTextRenderer.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace kxf::UI::DataView
{
	bool BitmapTextValue::FromAny(const wxAny& value)
	{
		return TextValue::FromAny(value) || BitmapValue::FromAny(value) || value.GetAs(this);
	}
}

namespace kxf::UI::DataView
{
	bool BitmapTextRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}

	void BitmapTextRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		const bool centerTextV = m_Value.ContainsOption(BitmapTextValueOption::VCenterText);
		const int reservedWidth = m_Value.GetDefaultBitmapWidth();

		GetRenderEngine().DrawBitmapWithText(cellRect, cellState, 0, m_Value.GetText(), m_Value.GetBitmap(), centerTextV, reservedWidth);
	}
	Size BitmapTextRenderer::GetCellSize() const
	{
		RenderEngine renderEngine = GetRenderEngine();

		Size size;
		if (m_Value.HasText())
		{
			size += renderEngine.GetTextExtent(m_Value.GetText());
		}
		if (m_Value.HasBitmap())
		{
			const GDIBitmap& bitmap = m_Value.GetBitmap();
			size.Width() += bitmap.GetWidth() + GetRenderEngine().FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.Height() < bitmap.GetHeight())
			{
				size.Height() = bitmap.GetHeight();
			}
		}
		else if (m_Value.IsDefaultBitmapWidthSpecified())
		{
			size.Width() += m_Value.GetDefaultBitmapWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
		}
		return size;
	}
}
