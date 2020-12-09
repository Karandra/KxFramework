#include "stdafx.h"
#include "BitmapRenderer.h"
#include "kxf/Drawing/BitmapImage.h"

namespace kxf::UI::DataView
{
	bool BitmapValue::FromAny(const wxAny& value)
	{
		if (value.GetAs(&m_Bitmap) || value.GetAs(this))
		{
			return true;
		}
		else if (GDIIcon icon; value.GetAs(&icon))
		{
			m_Bitmap = icon.ToBitmap();
			return true;
		}
		else if (BitmapImage image; value.GetAs(&image))
		{
			m_Bitmap = image.ToBitmap();
			return true;
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	bool BitmapRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
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
