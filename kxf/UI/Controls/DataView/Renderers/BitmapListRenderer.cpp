#include "stdafx.h"
#include "BitmapListRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"

namespace
{
	int CalcSpacing(wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, 0)).GetWidth();
	}
	kxf::Size GetSmallIconSize(const kxf::UI::DataView::BitmapValueBase& bitmapValue)
	{
		const int x = bitmapValue.IsDefaultBitmapWidthSpecified() ? bitmapValue.GetDefaultBitmapWidth() : wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
		return {x, wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y)};
	}
}

namespace kxf::UI::DataView
{
	bool BitmapListValue::FromAny(const wxAny& value)
	{
		return TextValue::FromAny(value) || value.GetAs(&m_Bitmaps) || value.GetAs(this);
	}
}

namespace kxf::UI::DataView
{
	void BitmapListRendererBase::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		RenderEngine renderEngine = GetRenderEngine();

		int offsetX = 0;
		if (size_t bitmapCount = GetBitmapCount(); bitmapCount != 0)
		{
			const Size smallIcon = GetSmallIconSize(m_BitmapValueBase);
			const int spacing = CalcSpacing(GetView(), m_BitmapValueBase.GetSpacing());

			for (size_t i = 0; i < bitmapCount; i++)
			{
				GDIBitmap bitmap = GetBitmap(i);
				if (bitmap || !m_BitmapValueBase.ShouldDrawInvalidBitmaps())
				{
					const Size bitmapSize = bitmap ? Size(bitmap.GetSize()) : smallIcon;

					Rect bitmapRect = cellRect;
					bitmapRect.SetX(cellRect.GetX() + offsetX);
					bitmapRect.SetWidth(bitmapSize.GetWidth());
					bitmapRect.SetHeight(std::clamp(bitmapSize.GetHeight(), 0, cellRect.GetHeight()));

					// Don't draw images with invalid indexes, but count them as drawn to allow spaces.
					if (bitmap)
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
	Size BitmapListRendererBase::GetCellSize() const
	{
		Size totalSize;
		if (m_TextValue.HasText())
		{
			RenderEngine renderEngine = GetRenderEngine();

			totalSize = renderEngine.GetTextExtent(m_TextValue.GetText());
			totalSize.Width() += renderEngine.GetInterTextSpacing();
		}
		if (size_t bitmapCount = GetBitmapCount(); bitmapCount != 0)
		{
			const Size smallIcon = GetSmallIconSize(m_BitmapValueBase);
			const int spacing = CalcSpacing(GetView(), m_BitmapValueBase.GetSpacing());

			for (size_t i = 0; i < bitmapCount; i++)
			{
				GDIBitmap bitmap = GetBitmap(i);
				if (bitmap || !m_BitmapValueBase.ShouldDrawInvalidBitmaps())
				{
					const Size bitmapSize = bitmap ? bitmap.GetSize() : smallIcon;

					totalSize.Width() += bitmapSize.GetWidth() + spacing;
					totalSize.Height() = std::max(totalSize.GetHeight(), bitmapSize.GetHeight());
				}
			}
		}
		return totalSize;
	}
}

namespace kxf::UI::DataView
{
	bool BitmapListRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}
}
