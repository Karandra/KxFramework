#include "stdafx.h"
#include "BitmapRenderer.h"
#include "kxf/Drawing/BitmapImage.h"

namespace kxf::UI::DataView
{
	bool BitmapValue::FromAny(Any value)
	{
		if (std::move(value).GetAs(m_Bitmap) || std::move(value).GetAs(*this))
		{
			return true;
		}
		else if (GDIIcon icon; std::move(value).GetAs(icon))
		{
			m_Bitmap = icon.ToGDIBitmap();
			return true;
		}
		else if (BitmapImage image; std::move(value).GetAs(image))
		{
			m_Bitmap = image.ToGDIBitmap();
			return true;
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	bool BitmapRenderer::SetDisplayValue(Any value)
	{
		m_Value.Clear();
		return m_Value.FromAny(std::move(value));
	}
	void BitmapRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		if (m_Value.HasBitmap())
		{
			GetRenderEngine().DrawBitmap(cellRect, cellState, m_Value.GetBitmap());
		}
	}
	Size BitmapRenderer::GetCellSize() const
	{
		if (m_Value.HasBitmap())
		{
			return m_Value.GetBitmap().GetSize();
		}
		return Size(0, 0);
	}
}
