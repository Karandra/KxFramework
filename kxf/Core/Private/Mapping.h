#pragma once
#include "../Common.h"
#include <wx/defs.h>
#include <wx/gdicmn.h>

namespace kxf::Private
{
	FlagSet<wxDirection> MapDirection(FlagSet<Direction> value) noexcept;
	FlagSet<Direction> MapDirection(FlagSet<wxDirection> wx) noexcept;

	FlagSet<wxOrientation> MapOrientation(FlagSet<Orientation> value) noexcept;
	FlagSet<Orientation> MapOrientation(FlagSet<wxOrientation> wx) noexcept;

	FlagSet<wxAlignment> MapAlignment(FlagSet<Alignment> value) noexcept;
	FlagSet<Alignment> MapAlignment(FlagSet<wxAlignment> wx) noexcept;

	FlagSet<wxEllipsizeFlags> MapEllipsizeFlag(FlagSet<EllipsizeFlag> value) noexcept;
	FlagSet<EllipsizeFlag> MapEllipsizeFlag(FlagSet<wxEllipsizeFlags> wx) noexcept;
}
