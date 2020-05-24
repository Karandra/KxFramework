#include "stdafx.h"
#include "DCOperations.h"

namespace kxf::Drawing
{
	Color GetAreaAverageColor(const wxDC& dc, const Rect& rect)
	{
		PackedRGBA<uint32_t> rgba;
		size_t pixelCount = 0;

		for (int y = 0; y < rect.GetHeight(); y++)
		{
			for (int x = 0; x < rect.GetWidth(); x++)
			{
				wxColour pixel;
				if (dc.GetPixel(rect.GetPosition() + Point(x, y), &pixel))
				{
					rgba.Red += pixel.Red();
					rgba.Green += pixel.Green();
					rgba.Blue += pixel.Blue();
					rgba.Alpha += pixel.Alpha();

					++pixelCount;
				}
			}
		}

		if (pixelCount != 0)
		{
			return Color::FromFixed8(rgba.Red / pixelCount, rgba.Green / pixelCount, rgba.Blue / pixelCount, rgba.Alpha / pixelCount);
		}
		return {};
	}
}
