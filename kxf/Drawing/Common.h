#pragma once
#include "kxf/Common.hpp"
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::Drawing
{
	constexpr int InvalidImageIndex = -1;
}

namespace kxf
{
	class GDIBitmap;
	class GDICursor;
	class Image;
	class GDIIcon;

	enum class ImageFormat
	{
		None = wxBITMAP_TYPE_INVALID,
		Any = wxBITMAP_TYPE_ANY,

		BMP = wxBITMAP_TYPE_BMP,
		ICO = wxBITMAP_TYPE_ICO,
		CUR = wxBITMAP_TYPE_CUR,
		ANI = wxBITMAP_TYPE_ANI,
		PNG = wxBITMAP_TYPE_PNG,
		GIF = wxBITMAP_TYPE_GIF,
		JPEG = wxBITMAP_TYPE_JPEG,
		TGA = wxBITMAP_TYPE_TGA,
		PNM = wxBITMAP_TYPE_PNM,
		IFF = wxBITMAP_TYPE_IFF,
		PCX = wxBITMAP_TYPE_PCX,
		XBM = wxBITMAP_TYPE_XBM,
		XPM = wxBITMAP_TYPE_XPM,
		TIF = wxBITMAP_TYPE_TIF,
		TIFF = wxBITMAP_TYPE_TIFF,
		PICT = wxBITMAP_TYPE_PICT,

		// Non-Wx formats
		RAW = 100,
		SVG,
		WEBP,
	};
}
namespace kxf
{
	// https://keithp.com/~keithp/porterduff/p253-porter.pdf
	// The description give a short equation on how the values of a resulting pixel are calculated.
	// R = Result, S = Source, D = Destination, colors premultiplied with alpha Ra, Sa, Da their alpha components
	enum class CompositionMode
	{
		None = -1,

		Clear, // R = 0
		Source, // R = S
		Over, // R = S + D*(1 - Sa)
		In, // R = S*Da
		Out, // R = S*(1 - Da)
		Atop, // R = S*Da + D*(1 - Sa)
		Dest, // R = D (essentially a no-op)
		DestOver, // R = S*(1 - Da) + D
		DestIn, // R = D*Sa
		DestOut, // R = D*(1 - Sa)
		DestAtop, // R = S*(1 - Da) + D*Sa
		Xor, // R = S*(1 - Da) + D*(1 - Sa)
		Add, // R = S + D
	};
	enum class AntialiasMode
	{
		None = -1,

		Default,
		BestAvailable = -2
	};
	enum class InterpolationQuality
	{
		None = 0,
		FastestAvailable = -1,
		BestAvailable = -2,

		Default = 1,
		NearestNeighbor,
		Bilinear,
		Bicubic,
		BoxAverage
	};

	// https://docs.microsoft.com/en-us/windows/win32/api/gdiplusenums/ne-gdiplusenums-hatchstyle
	enum class PolygonFill
	{
		OddEvenRule,
		WindingRule
	};
	enum class FloodFill
	{
		Surface,
		Border
	};
	enum class WrapMode
	{
		None = -1,

		Tile,
		FlipX,
		FlipY,
		FlipBoth
	};
	enum class HatchStyle
	{
		None = -1,

		Horizontal,
		Vertical,
		ForwardDiagonal,
		BackwardDiagonal,
		Cross,
		DiagonalCross,
		Percent05,
		Percent10,
		Percent20,
		Percent25,
		Percent30,
		Percent40,
		Percent50,
		Percent60,
		Percent70,
		Percent75,
		Percent80,
		Percent90,
		LightDownwardDiagonal,
		LightUpwardDiagonal,
		DarkDownwardDiagonal,
		DarkUpwardDiagonal,
		WideDownwardDiagonal,
		WideUpwardDiagonal,
		LightVertical,
		LightHorizontal,
		NarrowVertical,
		NarrowHorizontal,
		DarkVertical,
		DarkHorizontal,
		DashedDownwardDiagonal,
		DashedUpwardDiagonal,
		DashedHorizontal,
		DashedVertical,
		SmallConfetti,
		LargeConfetti,
		ZigZag,
		Wave,
		DiagonalBrick,
		HorizontalBrick,
		Weave,
		Plaid,
		Divot,
		DottedGrid,
		DottedDiamond,
		Shingle,
		Trellis,
		Sphere,
		SmallGrid,
		SmallCheckerBoard,
		LargeCheckerBoard,
		OutlinedDiamond,
		SolidDiamond
	};
	enum class DashStyle
	{
		None = -1,

		Dot,
		Dash,
		DashDot,
		DashDotDot,
		Custom
	};

	enum class PenStyle
	{
		None = -1,

		Solid,
		Hatch,
		Dash,
		Texture
	};
	enum class LineCap
	{
		None = -1,

		Flat,
		Round,
		Square
	};
	enum class LineJoin
	{
		None = -1,

		Miter,
		Bevel,
		Round
	};
	enum class LineAlignment
	{
		Center,
		Inset
	};

	enum class StockPen
	{
		Transparent,

		Black,
		BlackDashed,
		White,
		Cyan,
		Blue,
		Red,
		Green,
		Yellow,
		Gray,
		LightGray,
		MediumGray
	};
	enum class StockBrush
	{
		Transparent,

		Black,
		White,
		Cyan,
		Blue,
		Red,
		Green,
		Yellow,
		Gray,
		LightGray,
		MediumGray
	};
}
namespace kxf
{
	enum class FontEncoding
	{
		None = wxFONTENCODING_MAX,
		System = wxFONTENCODING_SYSTEM,
		Default = wxFONTENCODING_DEFAULT,

		UTF7 = wxFONTENCODING_UTF7,
		UTF8 = wxFONTENCODING_UTF8,

		UTF16LE = wxFONTENCODING_UTF16LE,
		UTF16BE = wxFONTENCODING_UTF16BE,
		UTF32LE = wxFONTENCODING_UTF32LE,
		UTF32BE = wxFONTENCODING_UTF32BE,

		ISO8859_1 = wxFONTENCODING_ISO8859_1,
		ISO8859_2 = wxFONTENCODING_ISO8859_2,
		ISO8859_3 = wxFONTENCODING_ISO8859_3,
		ISO8859_4 = wxFONTENCODING_ISO8859_4,
		ISO8859_5 = wxFONTENCODING_ISO8859_5,
		ISO8859_6 = wxFONTENCODING_ISO8859_6,
		ISO8859_7 = wxFONTENCODING_ISO8859_7,
		ISO8859_8 = wxFONTENCODING_ISO8859_8,
		ISO8859_9 = wxFONTENCODING_ISO8859_9,
		ISO8859_10 = wxFONTENCODING_ISO8859_10,
		ISO8859_11 = wxFONTENCODING_ISO8859_11,
		ISO8859_12 = wxFONTENCODING_ISO8859_12,
		ISO8859_13 = wxFONTENCODING_ISO8859_13,
		ISO8859_14 = wxFONTENCODING_ISO8859_14,
		ISO8859_15 = wxFONTENCODING_ISO8859_15,
		ISO2022_JP = wxFONTENCODING_ISO2022_JP,

		KIO8R = wxFONTENCODING_KOI8,
		KIO8U = wxFONTENCODING_KOI8_U,
		GB2312 = wxFONTENCODING_GB2312,
		BIG5 = wxFONTENCODING_BIG5,
		Johab = wxFONTENCODING_JOHAB,
		EUC_KR = wxFONTENCODING_EUC_KR,
		EUC_JP = wxFONTENCODING_EUC_JP,
		ShiftJIS = wxFONTENCODING_SHIFT_JIS,
		Bulgarian = wxFONTENCODING_BULGARIAN,
		Vietnamese = wxFONTENCODING_VIETNAMESE,

		CP437 = wxFONTENCODING_CP437,
		CP850 = wxFONTENCODING_CP850,
		CP852 = wxFONTENCODING_CP852,
		CP855 = wxFONTENCODING_CP855,
		CP866 = wxFONTENCODING_CP866,
		CP874 = wxFONTENCODING_CP874,
		CP932 = wxFONTENCODING_CP932,
		CP936 = wxFONTENCODING_CP936,
		CP949 = wxFONTENCODING_CP949,
		CP950 = wxFONTENCODING_CP950,
		CP1250 = wxFONTENCODING_CP1250,
		CP1251 = wxFONTENCODING_CP1251,
		CP1252 = wxFONTENCODING_CP1252,
		CP1253 = wxFONTENCODING_CP1253,
		CP1254 = wxFONTENCODING_CP1254,
		CP1255 = wxFONTENCODING_CP1255,
		CP1256 = wxFONTENCODING_CP1256,
		CP1257 = wxFONTENCODING_CP1257,
		CP1258 = wxFONTENCODING_CP1258,
		CP1361 = wxFONTENCODING_CP1361,
	};
	enum class FontFamily
	{
		None = -1,

		Default,
		Fantasy,
		Serif,
		SansSerif,
		Cursive,
		FixedWidth
	};
	enum class FontWeight
	{
		None = -1,

		Thin = 1,
		ExtraLight,
		Light,
		Normal,
		Medium,
		SemiBold,
		Bold,
		ExtraBold,
		Heavy,
		ExtraHeavy,
	};
	enum class FontSymbolicSize
	{
		Normal = 0,

		ExtraSmall = -3,
		VerySmall = -2,
		Small = -1,

		Large = 1,
		VeryLarge = 2,
		ExtraLarge = 3,
	};
	enum class FontStyle: uint32_t
	{
		Normal = 0,

		Italic = 1 << 1,
		Oblique = 1 << 2,
		Underline = 1 << 3,
		Strikethrough = 1 << 4,
	};
	KxFlagSet_Declare(FontStyle);
}
