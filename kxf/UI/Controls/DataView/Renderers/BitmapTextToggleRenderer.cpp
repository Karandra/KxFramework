#include "stdafx.h"
#include "BitmapTextToggleRenderer.h"
#include "BitmapRenderer.h"
#include "TextRenderer.h"
#include "ToggleRenderer.h"
#include "../View.h"
#include "kxf/Drawing/GDIRenderer/GDIGraphicsContext.h"

namespace kxf::UI::DataView
{
	bool BitmapTextToggleValue::FromAny(const wxAny& value)
	{
		return BitmapTextValue::FromAny(value) || ToggleValue::FromAny(value) || value.GetAs(this);
	}
}

namespace kxf::UI::DataView
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
			IGraphicsContext& gc = GetGraphicsContext();
			Rect toggleRect(cellRect.GetPosition(), GetRenderEngine().GetToggleSize());
			toggleRect.SetHeight(cellRect.GetHeight());

			offsetX += GetRenderEngine().DrawToggle(gc, toggleRect, cellState, m_Value.GetState(), m_Value.GetType()).GetWidth();
			offsetFromToggle = GetRenderEngine().FromDIPX(2);
		}
		if (m_Value.HasBitmap() || m_Value.HasText() || m_Value.IsDefaultBitmapWidthSpecified())
		{
			Rect rect = cellRect;
			rect.X() += offsetFromToggle;
			rect.Width() -= offsetFromToggle;

			const int reservedWidth = m_Value.GetDefaultBitmapWidth();
			const bool centerTextV = m_Value.ContainsOption(BitmapTextValueOption::VCenterText);
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
			size.Width() += toggleSize.GetWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.GetHeight() < toggleSize.GetHeight())
			{
				size.Height() = toggleSize.GetHeight();
			}
		}
		if (m_Value.HasText())
		{
			Size textExtent = renderEngine.GetTextExtent(m_Value.GetText());
			size.Width() += textExtent.GetWidth();
			if (size.GetHeight() < textExtent.GetHeight())
			{
				size.Height() = textExtent.GetHeight();
			}
		}
		if (m_Value.HasBitmap())
		{
			const GDIBitmap& bitmap = m_Value.GetBitmap();
			size.Width() += bitmap.GetWidth() + renderEngine.FromDIPX(renderEngine.GetInterTextSpacing());
			if (size.GetHeight() < bitmap.GetHeight())
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
