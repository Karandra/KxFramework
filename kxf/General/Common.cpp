#include "KxfPCH.h"
#include "Common.h"

namespace kxf::Private
{
	FlagSet<wxOrientation> MapOrientation(FlagSet<Orientation> value) noexcept
	{
		FlagSet<wxOrientation> wx;
		wx.Add(wxOrientation::wxHORIZONTAL, value & Orientation::Horizontal);
		wx.Add(wxOrientation::wxVERTICAL, value & Orientation::Vertical);

		return wx;
	}
	FlagSet<Orientation> MapOrientation(FlagSet<wxOrientation> wx) noexcept
	{
		FlagSet<Orientation> value;
		value.Add(Orientation::Horizontal, value & wxOrientation::wxHORIZONTAL);
		value.Add(Orientation::Vertical, value & wxOrientation::wxVERTICAL);

		return value;
	}
}
