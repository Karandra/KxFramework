#include "KxStdAfx.h"
#include "BitmapListRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"
#include "TextRenderer.h"

namespace
{
	int CalcSpacing(wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, 0)).GetWidth();
	}
	wxSize GetSmallIconSize()
	{
		return {wxSystemSettings::GetMetric(wxSYS_SMALLICON_X), wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y)};
	}
}

namespace KxDataView2
{
	bool BitmapListRendererBase::SetValue(const wxAny& value)
	{
		return TextRenderer::GetValueAsString(value, m_Text);
	}

	void BitmapListRendererBase::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		RenderEngine renderEngine = GetRenderEngine();

		int offsetX = 0;
		if (size_t bitmapCount = GetBitmapCount(); bitmapCount != 0)
		{
			const wxSize smallIcon = GetSmallIconSize();
			const int spacing = CalcSpacing(GetView(), m_Spacing);

			for (size_t i = 0; i < bitmapCount; i++)
			{
				const wxBitmap& bitmap = GetBitmap(i);
				if (bitmap.IsOk() || !m_SkipInvalidBitmaps)
				{
					const wxSize bitmapSize = bitmap.IsOk() ? bitmap.GetSize() : smallIcon;

					wxRect bitmapRect = cellRect;
					bitmapRect.SetX(cellRect.GetX() + offsetX);
					bitmapRect.SetWidth(bitmapSize.GetWidth());
					bitmapRect.SetHeight(std::clamp(bitmapSize.GetHeight(), 0, cellRect.GetHeight()));

					// Don't draw images with invalid indexes, but count them as drawn to allow spaces.
					if (bitmap.IsOk())
					{
						renderEngine.DrawBitmap(bitmapRect, cellState, bitmap);
					}

					// Move right
					offsetX += bitmapSize.GetWidth() + spacing;
				}
			}
		}
		if (m_Text.IsEmpty())
		{
			offsetX += renderEngine.GetInterTextSpacing();
			GetRenderEngine().DrawText(cellRect, cellState, m_Text, offsetX);
		}
	}
	wxSize BitmapListRendererBase::GetCellSize() const
	{
		wxSize totalSize;
		if (!m_Text.IsEmpty())
		{
			RenderEngine renderEngine = GetRenderEngine();

			totalSize = renderEngine.GetTextExtent(m_Text);
			totalSize.x += renderEngine.GetInterTextSpacing();
		}
		if (size_t bitmapCount = GetBitmapCount(); bitmapCount != 0)
		{
			const wxSize smallIcon = GetSmallIconSize();
			const int spacing = CalcSpacing(GetView(), GetSpacing());

			for (size_t i = 0; i < bitmapCount; i++)
			{
				const wxBitmap& bitmap = GetBitmap(i);
				if (bitmap.IsOk() || !m_SkipInvalidBitmaps)
				{
					const wxSize bitmapSize = bitmap.IsOk() ? bitmap.GetSize() : smallIcon;

					totalSize.x += bitmapSize.GetWidth() + spacing;
					totalSize.y = std::max(totalSize.y, bitmapSize.GetHeight());
				}
			}
		}
		return totalSize;
	}
}

namespace KxDataView2
{
	bool BitmapListRenderer::SetValue(const wxAny& value)
	{
		m_Bitmaps.clear();
		if (value.GetAs(&m_Bitmaps))
		{
			return true;
		}
		return BitmapListRendererBase::SetValue(value);
	}
}

namespace KxDataView2
{
	bool ImageListRenderer::SetValue(const wxAny& value)
	{
		KxImageList* imageList = nullptr;
		if (value.GetAs(&imageList))
		{
			SetImageList(imageList);
			return true;
		}
		return BitmapListRendererBase::SetValue(value);
	}
}
