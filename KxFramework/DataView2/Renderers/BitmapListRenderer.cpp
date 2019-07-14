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
	wxSize GetSmallIconSize(const KxDataView2::BitmapValueBase& bitmapValue)
	{
		const int x = bitmapValue.IsDefaultBitmapWidthSpecified() ? bitmapValue.GetDefaultBitmapWidth() : wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
		return wxSize(x, wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y));
	}
}

namespace KxDataView2
{
	bool BitmapListValue::FromAny(const wxAny& value)
	{
		return TextValue::FromAny(value) || value.GetAs(&m_Bitmaps) || value.GetAs(this);
	}
	bool ImageListValue::FromAny(const wxAny& value)
	{
		if (TextValue::FromAny(value) || value.GetAs(this))
		{
			return true;
		}
		else if (const KxImageList* imageList = nullptr; value.GetAs(&imageList))
		{
			KxWithImageList::SetImageList(imageList);
			return true;
		}
		return false;
	}
}

namespace KxDataView2
{
	void BitmapListRendererBase::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		RenderEngine renderEngine = GetRenderEngine();

		int offsetX = 0;
		if (size_t bitmapCount = GetBitmapCount(); bitmapCount != 0)
		{
			const wxSize smallIcon = GetSmallIconSize(m_BitmapValueBase);
			const int spacing = CalcSpacing(GetView(), m_BitmapValueBase.GetSpacing());

			for (size_t i = 0; i < bitmapCount; i++)
			{
				const wxBitmap& bitmap = GetBitmap(i);
				if (bitmap.IsOk() || !m_BitmapValueBase.ShouldDrawInvalidBitmaps())
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
		if (m_TextValue.HasText())
		{
			offsetX += renderEngine.GetInterTextSpacing();
			renderEngine.DrawText(cellRect, cellState, m_TextValue.GetText(), offsetX);
		}
	}
	wxSize BitmapListRendererBase::GetCellSize() const
	{
		wxSize totalSize;
		if (m_TextValue.HasText())
		{
			RenderEngine renderEngine = GetRenderEngine();

			totalSize = renderEngine.GetTextExtent(m_TextValue.GetText());
			totalSize.x += renderEngine.GetInterTextSpacing();
		}
		if (size_t bitmapCount = GetBitmapCount(); bitmapCount != 0)
		{
			const wxSize smallIcon = GetSmallIconSize(m_BitmapValueBase);
			const int spacing = CalcSpacing(GetView(), m_BitmapValueBase.GetSpacing());

			for (size_t i = 0; i < bitmapCount; i++)
			{
				const wxBitmap& bitmap = GetBitmap(i);
				if (bitmap.IsOk() || !m_BitmapValueBase.ShouldDrawInvalidBitmaps())
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
		if (!m_Value.FromAny(value))
		{
			m_Value.Clear();
			return false;
		}
		return true;
	}
}

namespace KxDataView2
{
	bool ImageListRenderer::SetValue(const wxAny& value)
	{
		if (!m_Value.FromAny(value))
		{
			m_Value.Clear();
			return false;
		}
		return true;
	}
}
