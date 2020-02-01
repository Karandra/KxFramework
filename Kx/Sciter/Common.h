#pragma once
#include "Kx/Common.hpp"
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

		// Relative
		em, // Relative to the font-size of the element (2em means 2 times the size of the current font)
		ex, // Relative to the x-height of the current font (rarely used)
		ch, // Relative to width of the "0" (zero)
		rem, // Relative to font-size of the root element
		vw, // Relative to 1% of the width of the viewport
		vh, // Relative to 1% of the height of the viewport
		vmin, // Relative to 1% of viewport's smaller dimension
		vmax, // Relative to 1% of viewport's larger dimension
		Percent, // Relative to the parent element
	};
}

namespace KxEnumClassOperations
{
}

namespace KxSciter
{
	wxStringView SizeUnitToString(SizeUnit unit);
}