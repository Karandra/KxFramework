#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/FlagSet.h"
#include <wx/defs.h>
#include <wx/gdicmn.h>
enum wxOrientation;

namespace kxf
{

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

	enum class MemoryProtection: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Execute = 1 << 2,
		CopyOnWrite = 1 << 3,

		RW = Read|Write,
		RX = Read|Execute,
		RWX = Read|Write|Execute,
	};
	KxFlagSet_Declare(MemoryProtection);

	enum class SortOrder
	{
		None = -1,
		Ascending,
		Descending
	};
}

namespace kxf::Private
{
	FlagSet<wxOrientation> MapOrientation(FlagSet<Orientation> value) noexcept;
	FlagSet<Orientation> MapOrientation(FlagSet<wxOrientation> wx) noexcept;
}
