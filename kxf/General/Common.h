#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/FlagSet.h"
#include <wx/defs.h>
#include <wx/gdicmn.h>

namespace kxf
{
	enum class VersionType
	{
		None = -1,

		Default,
		DateTime,
	};
	enum class UnitNameFlag: uint32_t
	{
		None = 0,

		Abbreviated = 1 << 0,
		GenetiveCase = 1 << 1,
	};
	KxDeclareFlagSet(UnitNameFlag);

	enum class CornerAlignment
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

	enum class EllipsizeMode
	{
		None = wxELLIPSIZE_NONE,
		Start = wxELLIPSIZE_START,
		Middle = wxELLIPSIZE_MIDDLE,
		End = wxELLIPSIZE_END,
	};

	enum class Orientation: uint32_t
	{
		None = 0,

		Horizontal = wxHORIZONTAL,
		Vertical = wxVERTICAL,
		Both = wxBOTH,
	};
	KxDeclareFlagSet(Orientation);

	enum class Alignment: uint32_t
	{
		None = 0,
		Invalid = static_cast<uint32_t>(wxAlignment::wxALIGN_INVALID),

		Left = wxALIGN_LEFT,
		Right = wxALIGN_RIGHT,
		Top = wxALIGN_TOP,
		Bottom = wxALIGN_BOTTOM,

		CenterVertical = wxALIGN_CENTER_VERTICAL,
		CenterHorizontal = wxALIGN_CENTER_HORIZONTAL,
		Center = CenterVertical|CenterHorizontal,
	};
	KxDeclareFlagSet(Alignment);

	enum class Direction: uint32_t
	{
		None = 0,

		Left = wxLEFT,
		Right = wxRIGHT,
		Up = wxUP,
		Down = wxDOWN,
	};
	KxDeclareFlagSet(Direction);
}

namespace kxf
{
	constexpr wxOrientation MapOrientation(Orientation value) noexcept
	{
		return static_cast<wxOrientation>(value);
	}
	constexpr Orientation MapOrientation(wxOrientation value) noexcept
	{
		return static_cast<Orientation>(value);
	}
}
