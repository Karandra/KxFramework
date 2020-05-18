#include "stdafx.h"
#include "BitmapTextToggleRenderer.h"
#include "BitmapRenderer.h"
#include "TextRenderer.h"
#include "ToggleRenderer.h"
#include "../View.h"

namespace KxFramework::UI::DataView
{
	bool BitmapTextToggleValue::FromAny(const wxAny& value)
	{
		return BitmapTextValue::FromAny(value) || ToggleValue::FromAny(value) || value.GetAs(this);
	}
}

namespace KxFramework::UI::DataView
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
		m_Value.Clear();
		return m_Value.FromAny(value);
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
		if (m_Value.HasBitmap() || m_Value.HasText() || m_Value.IsDefaultBitmapWidthSpecified())
		{
			wxRect rect = cellRect;
			rect.x += offsetFromToggle;
			rect.width -= offsetFromToggle;

			const int reservedWidth = m_Value.GetDefaultBitmapWidth();
			const bool centerTextV = m_Value.IsOptionEnabled(BitmapTextValueOption::VCenterText);
			GetRenderEngine().DrawBitmapWithText(rect, cellState, offsetX, m_Value.GetText(), m_Value.GetBitmap(), centerTextV, reservedWidth);
		}
	}
	wxSize BitmapTextToggleRenderer::GetCellSize() const
	{
		RenderEngine renderEngine = GetRenderEngine();

		wxSize size;
		if (m_Value.HasType())
		{
			wxSize toggleSize = renderEngine.GetToggleSize();
			size.x += toggleSize.x + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.y < toggleSize.y)
			{
				size.y = toggleSize.y;
			}
		}
		if (m_Value.HasText())
		{
			wxSize textExtent = renderEngine.GetTextExtent(m_Value.GetText());
			size.x += textExtent.x;
			if (size.y < textExtent.y)
			{
				size.y = textExtent.y;
			}
		}
		if (m_Value.HasBitmap())
		{
			const wxBitmap& bitmap = m_Value.GetBitmap();
			size.x += bitmap.GetWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.y < bitmap.GetHeight())
			{
				size.y = bitmap.GetHeight();
			}
		}
		else if (m_Value.IsDefaultBitmapWidthSpecified())
		{
			size.x += m_Value.GetDefaultBitmapWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
		}
		return size;
	}
}
