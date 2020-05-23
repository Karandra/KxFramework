#pragma once
#include <Kx/Common.hpp>
#include "Kx/General/FlagSet.h"
enum wxSeekMode: int;
enum wxStreamError: int;

namespace KxFramework
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
	enum class UnitNameFlag
	{
		None = 0,

		Abbreviated = 1 << 0,
		GenetiveCase = 1 << 1,
	};
	enum class Orientation
	{
		None = 0,

		Horizontal = wxOrientation::wxHORIZONTAL,
		Vertical = wxOrientation::wxVERTICAL,
		Both = wxOrientation::wxBOTH,
	};

	Kx_DeclareFlagSet(UnitNameFlag);
	Kx_DeclareFlagSet(Orientation);
}

namespace KxFramework
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
