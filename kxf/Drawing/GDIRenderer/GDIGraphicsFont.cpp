#include "stdafx.h"
#include "GDIGraphicsFont.h"
#include "GDIGraphicsContext.h"

namespace kxf
{
	GraphicsFontMetrics GDIGraphicsFont::GetMetrics() const
	{
		GraphicsFontMetrics metrics;
		if (auto context = m_Renderer->CreateMeasuringContext())
		{
			return context->GetTextExtent(wxS('W')).GetFontMetrics();
		}
		else
		{
			auto size = m_Font.GetPixelSize();
			metrics.AverageWidth = size.GetWidth();
			metrics.Height = size.GetHeight();
		}
		return metrics;
	}
}
