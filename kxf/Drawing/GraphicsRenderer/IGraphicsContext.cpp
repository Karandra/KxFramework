#include "stdafx.h"
#include "IGraphicsContext.h"
#include "../GDIRenderer/GDIBitmap.h"
#include "../GDIRenderer/GDIMemoryContext.h"

namespace kxf
{
	GDIBitmap IGraphicsContext::DrawGDIOnBitmap(const RectF& rect, std::function<void(GDIContext& dc)> func, bool forceAlpha)
	{
		if (!rect.IsEmpty())
		{
			// Draw GDI content on a memory DC to get a bitmap
			GDIBitmap bitmap(rect.GetSize(), ColorDepthDB::BPP32);
			{
				GDIMemoryContext dc(bitmap);

				// Set font
				if (auto font = GetFont())
				{
					dc.SetFont(font->ToFont());
				}

				// Set brush (solid only for now)
				if (auto brush = GetBrush())
				{
					if (auto solidBrush = brush->QueryInterface<IGraphicsSolidBrush>())
					{
						dc.SetBrush(solidBrush->GetColor());
					}
				}

				// Set pen (solid only for now)
				if (auto pen = GetPen())
				{
					GDIPen penGDI(pen->GetColor(), pen->GetWidth());
					penGDI.SetSolid();
					penGDI.SetJoin(pen->GetLineJoin());
					penGDI.SetCap(pen->GetLineCap());

					dc.SetPen(penGDI);
				}

				// Call the actual drawing routine
				dc.ResetBoundingBox();
				std::invoke(func, dc);

				// Avoid drawing this bitmap on the context entirely if nothing has been drawn on the DC
				if (dc.GetBoundingBox().IsEmpty())
				{
					return {};
				}
			}

			// The context implementation can use this bitmap to draw it any way it need
			if (forceAlpha)
			{
				bitmap.ForceAlpha();
			}
			return bitmap;
		}
		return {};
	}
}
