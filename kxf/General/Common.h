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
	KxFlagSet_Declare(UnitNameFlag);

	enum class ClockDirection
	{
		Clockwise = 1,
		CounterClockwise = -1
	};

	enum class CornerAlignment
	{
		None = 0,

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

	enum class EllipsizeFlag: uint32_t
	{
		None = 0,

		ExpandTabs = wxELLIPSIZE_FLAGS_EXPAND_TABS,
		ProcessMnemonics = wxELLIPSIZE_FLAGS_PROCESS_MNEMONICS
	};
	KxFlagSet_Declare(EllipsizeFlag);

	enum class Orientation: uint32_t
	{
		None = 0,

		Horizontal = wxHORIZONTAL,
		Vertical = wxVERTICAL,
		Both = wxBOTH,
	};
	KxFlagSet_Declare(Orientation);

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
	KxFlagSet_Declare(Alignment);

	enum class Direction: uint32_t
	{
		None = 0,

		Left = wxLEFT,
		Right = wxRIGHT,
		Up = wxUP,
		Down = wxDOWN,
	};
	KxFlagSet_Declare(Direction);

	enum class LineBreakFormat
	{
		None = -1,

		LF,
		CRLF,
		CR,

		Unix = LF,
		Windows = CRLF,
		Macintosh = CR
	};
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
