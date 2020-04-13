#pragma once
#include "Common.h"
#include <wx/defs.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework
{
	enum class StdButton: uint32_t
	{
		WX_LAST_STD = wxSETUP,
		None = 0,

		// Standard buttons
		OK = wxOK,
		Cancel = wxCANCEL,
		Yes = wxYES,
		No = wxNO,
		Apply = wxAPPLY,
		Close = wxCLOSE,
		Help = wxHELP,
		Forward = wxFORWARD,
		Backward = wxBACKWARD,
		Reset = wxRESET,
		More = wxMORE,
		Setup = wxSETUP,

		// Extra buttons
		Abort = WX_LAST_STD << 1,
		Retry = WX_LAST_STD << 2,
		Save = WX_LAST_STD << 3,
		DoNotSave = WX_LAST_STD << 4,
		ContextHelp = WX_LAST_STD << 5,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(StdButton);
	}
}

namespace KxFramework::UI
{
	constexpr inline StdButton FromWxStdButton(int stdWxButton) noexcept
	{
		return FromInt<StdButton>(stdWxButton);
	}
	constexpr inline int ToWxStdButton(StdButton stdButton) noexcept
	{
		return ToInt(stdButton);
	}
}
