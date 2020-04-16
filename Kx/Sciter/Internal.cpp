#include "stdafx.h"
#include "Internal.h"

namespace KxFramework::Sciter
{
	std::vector<BYTE> ToSciterUTF8(const String& text)
	{
		if (!text.IsEmpty())
		{
			auto utf8 = text.ToUTF8();
			constexpr size_t bomLength = 3;

			// Copy to buffer and add BOM sequence to force Sciter recognize UTF-8
			std::vector<BYTE> buffer;
			buffer.resize(utf8.length() + bomLength);
			buffer[0] = 0xEF;
			buffer[1] = 0xBB;
			buffer[2] = 0xBF;
			memcpy(buffer.data() + bomLength, utf8.data(), utf8.length());

			return buffer;
		}
		return {};
	}

	SC_COLOR CreateSciterColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		return GetGrapchicsAPI()->RGBA(r, g, b, a);
	}
	SC_COLOR CreateSciterColor(const wxColour& color)
	{
		if (color.IsOk())
		{
			return CreateSciterColor(color.Red(), color.Green(), color.Blue(), color.Alpha());
		}
		else
		{
			return CreateSciterColor(0, 0, 0, 255);
		}
	}
}
