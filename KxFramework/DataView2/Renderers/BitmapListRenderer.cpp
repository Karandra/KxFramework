#include "KxStdAfx.h"
#include "BitmapListRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"

namespace
{
	int CalcSpacing(wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, 0)).GetWidth();
	}
}

namespace KxDataView2
{
	void BitmapListRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		const size_t bitmapCount = GetBitmapCount();
		if (bitmapCount != 0)
		{
			const int spacing = CalcSpacing(GetView(), m_Spacing);

			int index = 0;
			for (size_t i = 0; i < bitmapCount; i++)
			{
				wxBitmap bitmap = GetBitmap(i);
				if (bitmap.IsOk() || !m_SkipInvalidBitmaps)
				{
					wxRect bitmapRect = cellRect;
					bitmapRect.SetX(cellRect.GetX() + (index * (bitmap.GetWidth() + spacing)));
					bitmapRect.SetWidth(bitmap.GetWidth());
					bitmapRect.SetHeight(std::clamp(bitmap.GetHeight(), 0, cellRect.GetHeight()));

					// Don't draw images with invalid indexes, but count them as drawn to allow spaces.
					if (bitmap.IsOk())
					{
						GetRenderEngine().DrawBitmap(bitmapRect, cellState, bitmap);
					}
					index++;
				}
			}
		}
	}
	wxSize BitmapListRenderer::GetCellSize() const
	{
		const size_t bitmapCount = GetBitmapCount();
		if (bitmapCount != 0)
		{
			int totalWidth = 0;
			int maxHeight = 0;
			for (size_t i = 0; i < bitmapCount; i++)
			{
				wxBitmap bitmap = GetBitmap(i);
				if (bitmap.IsOk())
				{
					totalWidth += bitmap.GetWidth();
					maxHeight = std::max(maxHeight, bitmap.GetHeight());
				}
			}
			return wxSize(totalWidth, maxHeight);
		}
		return wxSize(0, 0);
	}
}

namespace KxDataView2
{
	bool ImageListRenderer::SetValue(const wxAny& value)
	{
		return true;
	}
	wxSize ImageListRenderer::GetCellSize() const
	{
		if (const KxImageList* imageList = GetImageList())
		{
			const int spacing = CalcSpacing(GetView(), GetSpacing());

			wxSize size = imageList->GetSize();
			size.x = (size.x + spacing) * imageList->GetImageCount();
			return size;
		}
		return wxSize(0, 0);
	}
}
