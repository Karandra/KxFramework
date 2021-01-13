#include "stdafx.h"
#include "ImageRenderer.h"
#include "kxf/Drawing/BitmapImage.h"

namespace kxf::UI::DataView
{
	bool ImageValue::FromAny(Any value)
	{
		if (std::move(value).GetAs(m_Image) || std::move(value).GetAs(*this))
		{
			return true;
		}
		else if (BitmapImage image; std::move(value).GetAs(image))
		{
			m_Image = std::move(image);
			return true;
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	bool ImageRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(std::move(value));
	}
	void ImageRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		GetRenderEngine().DrawBitmap(cellRect, cellState, m_Value.GetImage());
	}
	Size ImageRenderer::GetCellSize() const
	{
		if (const BitmapImage& image = m_Value.GetImage())
		{
			return image.GetSize();
		}
		return {};
	}
}
