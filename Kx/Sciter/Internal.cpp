#include "KxStdAfx.h"
#include "Internal.h"

namespace KxSciter::Internal
{
	std::vector<BYTE> ToSciterUTF8(const wxString& text)
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
}
