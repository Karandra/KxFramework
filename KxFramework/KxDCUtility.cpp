#include "KxStdAfx.h"
#include "KxFramework/KxDCUtility.h"

namespace KxDCUtility
{
	KxColor GetAverageColor(const wxDC& dc, const wxRect& rect)
	{
		int r = 0;
		int g = 0;
		int b = 0;
		int a = 0;
		int pixelCount = 0;

		for (int y = 0; y < rect.GetHeight(); y++)
		{
			for (int x = 0; x < rect.GetWidth(); x++)
			{
				wxColour color;
				if (dc.GetPixel(rect.GetPosition() + wxPoint(x, y), &color))
				{
					r += color.Red();
					g += color.Green();
					b += color.Blue();
					a += color.Alpha();

					++pixelCount;
				}
			}
		}

		if (pixelCount != 0)
		{
			return KxColor().SetFixed8(r / pixelCount, g / pixelCount, b / pixelCount, a / pixelCount);
		}
		return {};
	}
}
