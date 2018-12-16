/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include <wx/defs.h>

enum KxIconType
{
	KxICON_NONE = wxICON_NONE,

	KxICON_WARNING = wxICON_WARNING,
	KxICON_ERROR = wxICON_ERROR,
	KxICON_QUESTION = wxICON_QUESTION,
	KxICON_INFO = wxICON_INFORMATION,
	KxICON_INFORMATION = KxICON_INFO,

	KxICON_SHIELD,
	KxICON_SHIELDHEADER,
	KxICON_SHIELDWARNING,
	KxICON_SHIELDERROR,
	KxICON_SHIELDSUCCESS,
	KxICON_SHIELDGRAY
};
