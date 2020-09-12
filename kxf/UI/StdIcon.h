#pragma once
#include "kxf/Utility/Common.h"
#include <wx/defs.h>
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	enum class StdIcon: uint32_t
	{
		None = 0,

		// Icons
		Error = 1 << 0,
		Warning = 1 << 1,
		Question = 1 << 2,
		Information = 1 << 3,
		Authentication = 1 << 4,

		// Modifiers
		Shield = 1 << 5,
		Success = 1 << 6,
		Neutral = 1 << 7,
		Header = 1 << 8,
	};
	KxFlagSet_Declare(StdIcon);
}

namespace kxf::UI
{
	constexpr inline FlagSet<StdIcon> FromWxStdIcon(int stdWxIcon) noexcept
	{
		FlagSet<StdIcon> stdIcon;
		stdIcon.Add(StdIcon::Error, stdWxIcon & wxICON_ERROR);
		stdIcon.Add(StdIcon::Warning, stdWxIcon & wxICON_WARNING);
		stdIcon.Add(StdIcon::Question, stdWxIcon & wxICON_QUESTION);
		stdIcon.Add(StdIcon::Information, stdWxIcon & wxICON_INFORMATION);
		stdIcon.Add(StdIcon::Authentication, stdWxIcon & wxICON_AUTH_NEEDED);

		return stdIcon;
	}
	constexpr inline int ToWxStdIcon(FlagSet<StdIcon> stdIcon) noexcept
	{
		int wxStdIcon = wxICON_NONE;
		Utility::AddFlagRef(wxStdIcon, wxICON_ERROR, stdIcon & StdIcon::Error);
		Utility::AddFlagRef(wxStdIcon, wxICON_WARNING, stdIcon & StdIcon::Warning);
		Utility::AddFlagRef(wxStdIcon, wxICON_QUESTION, stdIcon & StdIcon::Question);
		Utility::AddFlagRef(wxStdIcon, wxICON_INFORMATION, stdIcon & StdIcon::Information);
		Utility::AddFlagRef(wxStdIcon, wxICON_AUTH_NEEDED, stdIcon & StdIcon::Authentication);

		return wxStdIcon;
	}
}
