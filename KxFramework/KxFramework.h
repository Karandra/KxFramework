#pragma once
#include "KxIncludes.h"
#include "KxMainDefs.h"
#include "KxTypedefs.h"

// Global objects
#if !defined wxNullRegion
extern KX_API const wxRegion wxNullRegion;
#endif
#if !defined wxNullRect
extern KX_API const wxRect wxNullRect;
#endif
#if !defined wxNullString
extern KX_API const wxString wxNullString;
#endif

// Libs
#pragma comment(lib, "Ws2_32.lib")

#pragma comment(lib, "wxbase31u")
#pragma comment(lib, "wxbase31u_net")
#pragma comment(lib, "wxmsw31u_core")
#pragma comment(lib, "wxmsw31u_adv")
#pragma comment(lib, "wxmsw31u_aui")
#pragma comment(lib, "wxmsw31u_webview")
#pragma comment(lib, "wxmsw31u_stc")
#pragma comment(lib, "wxmsw31u_html")
#pragma comment(lib, "wxscintilla")
#pragma comment(lib, "wxzlib")
#pragma comment(lib, "wxpng")
#pragma comment(lib, "wxjpeg")
#pragma comment(lib, "wxtiff")
#pragma comment(lib, "wxexpat")
#pragma comment(lib, "wxregexu")

// Main library includes
#include <KxFramework/KxStandardID.h>
#include <KxFramework/KxButtonType.h>
#include <KxFramework/KxIconType.h>
#include "KxFramework/KxString.h"
#include "KxFramework/KxColor.h"
#include "KxFramework/KxMath.h"
