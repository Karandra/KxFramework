#include "stdafx.h"
#include "BitmapRenderer.h"

namespace kxf::UI::DataView
{
	bool BitmapValue::FromAny(const wxAny& value)
	{
		if (value.GetAs(&m_Bitmap) || value.GetAs(this))
		{
			return true;
		}
		else if (wxIcon icon; value.GetAs(&icon))
		{
			m_Bitmap.CopyFromIcon(icon, wxBitmapTransparency_Auto);
			return true;
		}
		else if (wxImage image; value.GetAs(&image))
		{
			m_Bitmap = wxBitmap(image, 32);
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
