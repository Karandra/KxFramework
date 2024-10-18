#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/UniversallyUniqueID.h"

namespace kxf::ImageOption
{
	// Common
	extern const XChar* FileName;
	extern const XChar* Quality;

	extern const XChar* DPI;
	extern const XChar* Resolution;
	extern const XChar* ResolutionX;
	extern const XChar* ResolutionY;
	extern const XChar* ResolutionUnit;

	extern const XChar* DesiredWidth;
	extern const XChar* DesiredHeight;

	extern const XChar* OriginalWidth;
	extern const XChar* OriginalHeight;

	namespace PNG
	{
		extern const XChar* Format;
		extern const XChar* BitDepth;
		extern const XChar* Filter;

		extern const XChar* CompressionLevel;
		extern const XChar* CompressionMemoryLevel;
		extern const XChar* CompressionStrategy;
		extern const XChar* CompressionBufferSize;
	}
	namespace GIF
	{
		extern const XChar* Comment;
		extern const XChar* Transparency;
		extern const XChar* TransparencyHighlight;
		extern const XChar* TransparencyUnchanged;
	}
	namespace SVG
	{
		extern const XChar* CurrentTime;
		extern const XChar* HasAnimation;
		extern const XChar* AnimationDuration;
	}
	namespace TIFF
	{
		extern const XChar* BitsPerSample;
		extern const XChar* SamplesPerPixel;
		extern const XChar* Compression;
		extern const XChar* Photometric;
		extern const XChar* ImageDescriptor;
	}
	namespace Cursor
	{
		extern const XChar* HotSpotX;
		extern const XChar* HotSpotY;
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
