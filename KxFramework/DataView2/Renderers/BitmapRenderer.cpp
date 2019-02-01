#include "KxStdAfx.h"
#include "BitmapRenderer.h"

namespace KxDataView2
{
	bool BitmapRenderer::GetValueAsBitmap(const wxAny& value, wxBitmap& bitmap)
	{
		if (!value.GetAs(&bitmap))
		{
			wxIcon icon;
			if (value.GetAs(&icon))
			{
				bitmap.CopyFromIcon(icon);
				return true;
			}
			else
			{
				wxImage image;
				if (value.GetAs(&image))
				{
					bitmap = wxBitmap(image, 32);
					return true;
				}
			}
			return false;
		}
		else
		{
			return true;
		}
	}

	bool BitmapRenderer::SetValue(const wxAny& value)
	{
		m_Bitmap = wxNullBitmap;
		return GetValueAsBitmap(value, m_Bitmap);
	}
	void BitmapRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		GetRenderEngine().DrawBitmap(cellRect, cellState, m_Bitmap);
	}
	wxSize BitmapRenderer::GetCellSize() const
	{
		return m_Bitmap.IsOk() ? m_Bitmap.GetSize() : wxSize(0, 0);
	}
}
