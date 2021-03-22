#include "KxfPCH.h"
#include "Internal.h"

namespace kxf::Sciter
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

	String FSPathToSciterAddress(const FSPath& path)
	{
		return path ? String(wxS("file://")) + path.GetFullPath(FSPathNamespace::None, FSPathFormat::TrailingSeparator) : wxS("about:blank");
	}
	String URIToSciterAddress(const URI& uri)
	{
		if (!uri.HasScheme())
		{
			return String(wxS("file://")) + uri.BuildURI();
		}
		return uri.BuildURI();
	}

	SC_COLOR CreateSciterColor(kxf::PackedRGBA<uint8_t> color) noexcept
	{
		return GetGrapchicsAPI()->RGBA(color.Red, color.Green, color.Blue, color.Alpha);
	}
	SC_COLOR CreateSciterColor(const Color& color) noexcept
	{
		if (color)
		{
			return CreateSciterColor(color.GetFixed8());
		}
		else
		{
			return CreateSciterColor({0, 0, 0, 255});
		}
	}
}
