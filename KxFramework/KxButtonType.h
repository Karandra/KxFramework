/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include <wx/defs.h>

enum KxButtonType
{
	KxBTN_LAST_WXSTD = wxSETUP,
	KxBTN_NONE = 0,

	// Standard buttons
	KxBTN_OK = wxOK,
	KxBTN_CANCEL = wxCANCEL,
	KxBTN_YES = wxYES,
	KxBTN_NO = wxNO,
	KxBTN_APPLY = wxAPPLY,
	KxBTN_CLOSE = wxCLOSE,
	KxBTN_HELP = wxHELP,
	KxBTN_FORWARD = wxFORWARD,
	KxBTN_BACKWARD = wxBACKWARD,
	KxBTN_RESET = wxRESET,
	KxBTN_MORE = wxMORE,
	KxBTN_SETUP = wxSETUP,

	// Extra buttons
	KxBTN_RETRY = KxBTN_LAST_WXSTD << 1,
	KxBTN_CONTEXT_HELP = KxBTN_LAST_WXSTD << 2,
	KxBTN_SAVE = KxBTN_LAST_WXSTD << 3,
	KxBTN_DONT_SAVE = KxBTN_LAST_WXSTD << 4,
};
