#pragma once
#include <KxFramework/KxEnumClassOperations.h>

namespace KxSciter
{
	using namespace KxEnumClassOperations;

	enum class FontSmoothing
	{
		None = 0,
		SystemDefault,
		Standard,
		ClearType
	};
	enum class WindowFrame
	{
		Standard = 0,
		Solid,
		SolidWithShadow,
		Extended,
		Transparent
	};
	enum class WindowBlurBehind
	{
		None = 0,
		Auto,

		Light,
		UltraLight,

		Dark,
		UltraDark
	};
	enum class WindowRenderer
	{
		Default = 0,
		DirectX
	};

	enum class ElementInnerHTML
	{
		Replace,
		InsertBefore,
		InsertAfter,
	};
	enum class ElementOuterHTML
	{
		Replace,
		InsertBefore,
		InsertAfter,
	};

	enum class SizeUnit
	{
		None = 0,

		// Absolute
		cm, // Centimeter
		mm, // Millimeter
		in, // Inch
		px, // Pixel
		pt, // Point
		pc, // Pica
		dip, // Device-independent pixel

		// Relative
		em, // Relative to the font-size of the element (2em means 2 times the size of the current font)
		ex, // Relative to the x-height of the current font (rarely used)
		ch, // Relative to width of the "0" (zero)
		rem, // Relative to font-size of the root element
		vw, // Relative to 1% of the width of the viewport
		vh, // Relative to 1% of the height of the viewport
		vmin, // Relative to 1% of viewport's smaller dimension
		vmax, // Relative to 1% of viewport's larger dimension
		pr, // %, relative to the parent element
		sp, // %% "springs", a.k.a. flex units
	};

	enum class ScrollSource
	{
		Unknown = 0,
		Keyboard,
		Scrollbar,
		Animator,
		Wheel
	};

	struct TextMetrics
	{
		double MinWidth = 0;
		double MaxWidth = 0;
		double Height = 0;
		double Ascent = 0;
		double Descent = 0;
		int LineCount = 0;
	};

	enum class Alignment
	{
		Unspecified = 0,

		TopLeft = 7,
		TopCenter = 8,
		TopRight = 9,

		MiddleLeft = 4,
		MiddleCenter = 5,
		MiddleRight = 6,

		BottomLeft = 1,
		BottomCenter = 2,
		BottomRight = 3,
	};
}

namespace KxEnumClassOperations
{
	KxAllowEnumCastOp(KxSciter::Alignment);
}
