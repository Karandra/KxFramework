#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/UniversallyUniqueID.h"
#include <wx/image.h>

namespace kxf::ImageOption
{
	// Common
	constexpr auto FileName = wxS("FileName");
	constexpr auto Quality = wxS("quality");

	constexpr auto DPI = wxS("DPI");
	constexpr auto Resolution = wxS("Resolution");
	constexpr auto ResolutionX = wxS("ResolutionX");
	constexpr auto ResolutionY = wxS("ResolutionY");
	constexpr auto ResolutionUnit = wxS("ResolutionUnit");

	constexpr auto DesiredWidth = wxS("MaxWidth");
	constexpr auto DesiredHeight = wxS("MaxHeight");

	constexpr auto OriginalWidth = wxS("OriginalWidth");
	constexpr auto OriginalHeight = wxS("OriginalHeight");

	namespace PNG
	{
		constexpr auto Format = wxIMAGE_OPTION_PNG_FORMAT;
		constexpr auto BitDepth = wxIMAGE_OPTION_PNG_BITDEPTH;
		constexpr auto Filter = wxIMAGE_OPTION_PNG_FILTER;

		constexpr auto CompressionLevel = wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL;
		constexpr auto CompressionMemoryLevel = wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL;
		constexpr auto CompressionStrategy = wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY;
		constexpr auto CompressionBufferSize = wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE;
	}
	namespace GIF
	{
		constexpr auto Comment = wxIMAGE_OPTION_GIF_COMMENT;
		constexpr auto Transparency = wxIMAGE_OPTION_GIF_TRANSPARENCY;
		constexpr auto TransparencyHighlight = wxIMAGE_OPTION_GIF_TRANSPARENCY_HIGHLIGHT;
		constexpr auto TransparencyUnchanged = wxIMAGE_OPTION_GIF_TRANSPARENCY_UNCHANGED;
	}
	namespace SVG
	{
		constexpr auto CurrentTime = wxS("CurrentTime");
		constexpr auto HasAnimation = wxS("HasAnimation");
		constexpr auto AnimationDuration = wxS("AnimationDuration");
	}
	namespace TIFF
	{
		constexpr auto BitsPerSample = wxT("BitsPerSample");
		constexpr auto SamplesPerPixel = wxT("SamplesPerPixel");
		constexpr auto Compression = wxT("Compression");
		constexpr auto Photometric = wxT("Photometric");
		constexpr auto ImageDescriptor = wxT("ImageDescriptor");
	}
	namespace Cursor
	{
		constexpr auto HotSpotX = wxIMAGE_OPTION_CUR_HOTSPOT_X;
		constexpr auto HotSpotY = wxIMAGE_OPTION_CUR_HOTSPOT_Y;
	}
}

namespace kxf::ImageFormat
{
	constexpr UniversallyUniqueID None = {};
	constexpr UniversallyUniqueID Any = NativeUUID{0xc9a4865a, 0xc215, 0x4076, {0x8c, 0xe7, 0xf6, 0x28, 0x4d, 0xe2, 0xf0, 0x93}};

	constexpr UniversallyUniqueID BMP = NativeUUID{0x3186c554, 0x5b9e, 0x4e52, {0xaa, 0xc4, 0x20, 0xce, 0x4c, 0xa1, 0xf1, 0x44}};
	constexpr UniversallyUniqueID ICO = NativeUUID{0x3671cfe2, 0x63b5, 0x488c, {0x83, 0xd9, 0x71, 0xfb, 0x7d, 0x80, 0xfb, 0x36}};
	constexpr UniversallyUniqueID CUR = NativeUUID{0x930c850c, 0x1684, 0x4915, {0x90, 0xcc, 0x56, 0xac, 0x49, 0x1f, 0x34, 0x55}};
	constexpr UniversallyUniqueID ANI = NativeUUID{0xe3dd2593, 0xbbc9, 0x4465, {0x87, 0x2c, 0x75, 0x9c, 0x80, 0x61, 0xc9, 0x15}};
	constexpr UniversallyUniqueID PNG = NativeUUID{0xa9505315, 0x062d, 0x4417, {0xbd, 0x5b, 0xf4, 0x41, 0xbc, 0x60, 0x1c, 0xb8}};
	constexpr UniversallyUniqueID GIF = NativeUUID{0xf11222db, 0x471b, 0x4901, {0x99, 0x76, 0x6f, 0x7b, 0xb2, 0x5d, 0x32, 0x14}};
	constexpr UniversallyUniqueID TGA = NativeUUID{0x0a968957, 0x516b, 0x4836, {0xbe, 0x8f, 0x52, 0x71, 0x5f, 0xda, 0xc7, 0xdc}};
	constexpr UniversallyUniqueID PNM = NativeUUID{0xceaa1743, 0x9f0c, 0x4dd9, {0xb2, 0x4a, 0xe5, 0x17, 0x88, 0x51, 0x24, 0xef}};
	constexpr UniversallyUniqueID IFF = NativeUUID{0x82ffa8e5, 0x200c, 0x4b08, {0xab, 0x71, 0xe8, 0x1f, 0xff, 0x6d, 0x49, 0x70}};
	constexpr UniversallyUniqueID PCX = NativeUUID{0x6fe40fd1, 0x5a9d, 0x41d6, {0xb1, 0x62, 0x6f, 0xd9, 0x25, 0x8d, 0x8f, 0x1d}};
	constexpr UniversallyUniqueID XBM = NativeUUID{0x7579ab2b, 0x0a68, 0x444f, {0xbf, 0xc2, 0xd4, 0xc4, 0xc0, 0x8d, 0x21, 0xe0}};
	constexpr UniversallyUniqueID XPM = NativeUUID{0xec3920b0, 0xd70c, 0x48a9, {0x99, 0x5a, 0x84, 0x33, 0x8b, 0xa9, 0xc4, 0xf8}};
	constexpr UniversallyUniqueID RAW = NativeUUID{0x790508ae, 0xb14c, 0x4bb7, {0xa3, 0xd9, 0xc3, 0x8e, 0xee, 0xd8, 0x19, 0xa4}};
	constexpr UniversallyUniqueID SVG = NativeUUID{0xa12782ea, 0xf7a2, 0x4ca6, {0xb1, 0xac, 0xd7, 0x4f, 0xb6, 0x2b, 0x65, 0x55}};
	constexpr UniversallyUniqueID TIF = NativeUUID{0x1e83e788, 0xbe10, 0x4839, {0x94, 0x23, 0x20, 0xba, 0xae, 0x8a, 0x57, 0x97}};
	constexpr UniversallyUniqueID TIFF = NativeUUID{0x59b7bee0, 0x2aab, 0x493b, {0xa7, 0xea, 0x2f, 0xa7, 0xa9, 0xbd, 0xdc, 0xca}};
	constexpr UniversallyUniqueID PICT = NativeUUID{0x746a4886, 0x0288, 0x4293, {0xb9, 0x48, 0xe7, 0x12, 0x06, 0x14, 0x17, 0x5d}};
	constexpr UniversallyUniqueID JPEG = NativeUUID{0xc3874089, 0xd2eb, 0x409f, {0x92, 0x5c, 0x01, 0x24, 0xfb, 0xa3, 0xf4, 0x07}};
	constexpr UniversallyUniqueID WEBP = NativeUUID{0xe2f2c67b, 0xcd9b, 0x4d51, {0x97, 0xa3, 0x03, 0x75, 0x98, 0xb9, 0x7e, 0xe2}};
}
