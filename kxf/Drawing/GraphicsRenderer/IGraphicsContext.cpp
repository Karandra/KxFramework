#include "stdafx.h"
#include "IGraphicsContext.h"
#include "../GDIRenderer/GDIBitmap.h"
#include "../GDIRenderer/GDIMemoryContext.h"

namespace kxf
{
	GDIBitmap IGraphicsContext::DrawGDIOnBitmap(const RectF& rect, std::function<void(GDIContext& dc)> func)
	{
		if (!rect.IsEmpty())
		{
			// Draw GDI content on a memory DC to get a bitmap
			GDIBitmap bitmap(rect.GetSize(), ColorDepthDB::BPP32);
			{
				GDIMemoryContext dc(bitmap);
				std::invoke(func, dc);
			}

			// The context implementation can use this bitmap to draw it any way it need
			return bitmap;
		}
		return {};
	}
}
