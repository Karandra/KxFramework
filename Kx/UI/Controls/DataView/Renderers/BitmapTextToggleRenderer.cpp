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
	wxAny BitmapTextToggleRenderer::OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent)
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
	void BitmapTextToggleRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		int offsetX = 0;
		int offsetFromToggle = 0;

		if (m_Value.HasType())
		{
			Rect toggleRect(cellRect.GetPosition(), GetRenderEngine().GetToggleSize());
			toggleRect.SetHeight(cellRect.GetHeight());

			offsetX += GetRenderEngine().DrawToggle(GetGraphicsDC(), toggleRect, cellState, m_Value.GetState(), m_Value.GetType()).GetWidth();
			offsetFromToggle = GetRenderEngine().FromDIPX(2);
		}
		if (m_Value.HasBitmap() || m_Value.HasText() || m_Value.IsDefaultBitmapWidthSpecified())
		{
			Rect rect = cellRect;
			rect.X() += offsetFromToggle;
			rect.Width() -= offsetFromToggle;

			const int reservedWidth = m_Value.GetDefaultBitmapWidth();
			const bool centerTextV = m_Value.IsOptionEnabled(BitmapTextValueOption::VCenterText);
			GetRenderEngine().DrawBitmapWithText(rect, cellState, offsetX, m_Value.GetText(), m_Value.GetBitmap(), centerTextV, reservedWidth);
		}
	}
	Size BitmapTextToggleRenderer::GetCellSize() const
	{
		RenderEngine renderEngine = GetRenderEngine();

		Size size;
		if (m_Value.HasType())
		{
			Size toggleSize = renderEngine.GetToggleSize();
			size.X() += toggleSize.GetX() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.GetY() < toggleSize.GetY())
			{
				size.Y() = toggleSize.GetY();
			}
		}
		if (m_Value.HasText())
		{
			Size textExtent = renderEngine.GetTextExtent(m_Value.GetText());
			size.X() += textExtent.GetX();
			if (size.GetY() < textExtent.GetY())
			{
				size.Y() = textExtent.GetY();
			}
		}
		if (m_Value.HasBitmap())
		{
			const wxBitmap& bitmap = m_Value.GetBitmap();
			size.X() += bitmap.GetWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.GetY() < bitmap.GetHeight())
			{
				size.Y() = bitmap.GetHeight();
			}
		}
		else if (m_Value.IsDefaultBitmapWidthSpecified())
		{
			size.X() += m_Value.GetDefaultBitmapWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
		}
		return size;
	}
}
