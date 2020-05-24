#pragma once
#include <kxf/Common.hpp>
#include "kxf/General/FlagSet.h"
enum wxSeekMode: int;
enum wxStreamError: int;

namespace kxf
{
	enum class VersionType
	{
		None = -1,

		Default,
		DateTime,
	};
	enum class StreamSeekMode
	{
		FromStart,
		FromCurrent,
		FromEnd,
	};
	enum class StreamErrorCode
	{
		Success,
		EndOfStream,
		ReadError,
		WriteError
	};
	enum class UnitNameFlag: uint32_t
	{
		None = 0,

		Abbreviated = 1 << 0,
		GenetiveCase = 1 << 1,
	};
	Kx_DeclareFlagSet(UnitNameFlag);

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
	Kx_DeclareFlagSet(Orientation);
	Kx_DeclareFlagSet(wxOrientation);

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
	Kx_DeclareFlagSet(Alignment);
	Kx_DeclareFlagSet(wxAlignment);

	enum class Direction: uint32_t
	{
		None = 0,

		Left = wxLEFT,
		Right = wxRIGHT,
		Up = wxUP,
		Down = wxDOWN,
	};
	Kx_DeclareFlagSet(Direction);
	Kx_DeclareFlagSet(wxDirection);
}

namespace kxf
{
	std::optional<wxSeekMode> ToWxSeekMode(StreamSeekMode seekMode) noexcept;
	std::optional<StreamSeekMode> FromWxSeekMode(wxSeekMode seekMode) noexcept;

	std::optional<wxStreamError> ToWxStreamError(StreamErrorCode streamError) noexcept;
	std::optional<StreamErrorCode> FromWxStreamError(wxStreamError streamError) noexcept;

	constexpr wxOrientation MapOrientation(Orientation value) noexcept
	{
		return static_cast<wxOrientation>(value);
	}
	constexpr Orientation MapOrientation(wxOrientation value) noexcept
	{
		return static_cast<Orientation>(value);
	}
}
