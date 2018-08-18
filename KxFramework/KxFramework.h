#pragma once

// wxWidgets actively uses these functions
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

// For SimpleINI
#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING 1
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 1

/*  C++ standard library */
// C part
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cfloat>
#include <ctime>
#include <cctype>

// C++ part
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <stack>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <variant>
#include <locale>
#include <limits>

/* wxWidgets */
// General
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/help.h>
#include <wx/regex.h>

// Threading
#include <wx/thread.h>

// System
#include <wx/sysopt.h>

// Drawing
#include <wx/gdicmn.h>
#include <wx/region.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include <wx/renderer.h>
#include <wx/artprov.h>
#include <wx/wupdlock.h>

// Streams
#include <wx/mstream.h>

// Windows utilities
#include <wx/validate.h>
#include <wx/valnum.h>

// Windows
#include <wx/window.h>
#include <wx/windowid.h>
#include <wx/nativewin.h>

// Controls
#include <wx/bmpcbox.h>
#include <wx/tglbtn.h>
#include <wx/combo.h>
#include <wx/odcombo.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/dataview.h>
#include <wx/treelist.h>
#include <wx/headerctrl.h>
#include <wx/bookctrl.h>
#include <wx/simplebook.h>
#include <wx/clrpicker.h>
#include <wx/statbmp.h>
#include <wx/generic/statbmpg.h>
#include <wx/statline.h>
#include <wx/webview.h>
#include <wx/srchctrl.h>
#include <wx/infobar.h>
#include <wx/progdlg.h>
#include <wx/stc/stc.h>
#include <wx/richtooltip.h>
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/fs_mem.h>

// Libs
#pragma comment(lib, "Ws2_32.lib")

#pragma comment(lib, "wxbase31")
#pragma comment(lib, "wxbase31_net.lib")
#pragma comment(lib, "wxmsw31_core")
#pragma comment(lib, "wxmsw31_adv")
#pragma comment(lib, "wxmsw31_aui")
#pragma comment(lib, "wxmsw31_webview")
#pragma comment(lib, "wxmsw31_stc")
#pragma comment(lib, "wxmsw31_html")
#pragma comment(lib, "wxscintilla")
#pragma comment(lib, "wxzlib")
#pragma comment(lib, "wxpng")
#pragma comment(lib, "wxjpeg")
#pragma comment(lib, "wxtiff")
#pragma comment(lib, "wxexpat")
#pragma comment(lib, "wxregex")

// Global objects
#if !defined wxNullRegion
extern const wxRegion wxNullRegion;
#endif
#if !defined wxNullRect
extern const wxRect wxNullRect;
#endif
#if !defined wxNullString
extern const wxString wxNullString;
#endif

/* Enums */
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

/* Misc */
#define KX_COMCALL STDMETHODCALLTYPE

#if defined KXFRAMEWORK_EXPORT
#define KX_API __declspec(dllexport)
#else
#define KX_API __declspec(dllimport)
#endif

/* Typedefs */
using KxStringVector = std::vector<wxString>;
using KxStdStringVector = std::vector<std::string>;
using KxStdStringViewVector = std::vector<std::string_view>;
using KxStdWStringVector = std::vector<std::wstring>;
using KxStdWStringViewVector = std::vector<std::wstring_view>;

using KxIntVector = std::vector<int>;
using KxIntPtrVector = std::vector<intptr_t>;
using KxSizeTVector = std::vector<size_t>;

using KxInt8Vector = std::vector<int8_t>;
using KxInt16Vector = std::vector<int16_t>;
using KxInt32Vector = std::vector<int32_t>;
using KxInt64Vector = std::vector<int64_t>;

using KxUInt8Vector = std::vector<uint8_t>;
using KxUInt16Vector = std::vector<uint16_t>;
using KxUInt32Vector = std::vector<uint32_t>;
using KxUInt64Vector = std::vector<uint64_t>;

using KxAnyVector = std::vector<wxAny>;

/* KxFramework */
#include <KxFramework/KxStandardID.h>
#include "KxFramework/KxRTTI.h"
#include "KxFramework/KxString.h"
#include "KxFramework/KxColor.h"

