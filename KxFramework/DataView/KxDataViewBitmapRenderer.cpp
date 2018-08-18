#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewBitmapRenderer.h"

bool KxDataViewBitmapRenderer::GetValueAsBitmap(const wxAny& value, wxBitmap& bitmap)
{
	if (!value.GetAs<wxBitmap>(&bitmap))
	{
		wxIcon icon;
		if (value.GetAs<wxIcon>(&icon))
		{
			bitmap.CopyFromIcon(icon);
			return true;
		}
		else
		{
			wxImage image;
			if (value.GetAs<wxImage>(&image))
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

bool KxDataViewBitmapRenderer::SetValue(const wxAny& value)
{
	m_Bitmap = wxNullBitmap;
	return GetValueAsBitmap(value, m_Bitmap);
}

void KxDataViewBitmapRenderer::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	DoDrawBitmap(cellRect, cellState, m_Bitmap);
}
wxSize KxDataViewBitmapRenderer::GetCellSize() const
{
	if (m_Bitmap.IsOk())
	{
		return m_Bitmap.GetSize();
	}
	return wxSize(0, KxDataViewRenderer::GetCellSize().GetHeight());
}
