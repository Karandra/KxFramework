#include "stdafx.h"
#include "WxGraphicsFont.h"
#include "WxGraphicsContext.h"

namespace kxf
{
	GraphicsFontMetrics WxGraphicsFont::GetMetrics() const
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
