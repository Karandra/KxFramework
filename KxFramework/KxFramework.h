#pragma once

// wxWidgets actively uses these functions
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

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
#include <cinttypes>

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
#include <optional>
#include <locale>
#include <limits>

/* wxWidgets */
// Required by wxWidgets network module which doesn't link to this library itself for some reason
#pragma comment(lib, "WS2_32.lib")

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

// Misc
#define KX_COMCALL STDMETHODCALLTYPE

#if defined KXFRAMEWORK_LIBRARY
#define KX_API __declspec(dllexport)
#elif defined KXFRAMEWORK_PLUGIN
#define KX_API __declspec(dllimport)
#else
#define KX_API
#endif
