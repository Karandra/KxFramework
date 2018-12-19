#pragma once
#include "KxIncludes.h"
#include "KxMainDefs.h"
#include "KxTypedefs.h"

// Global objects
#if !defined KxNullWxRegion
extern KX_API const wxRegion KxNullWxRegion;
#endif
#if !defined KxNullWxRect
extern KX_API const wxRect KxNullWxRect;
#endif
#if !defined KxNullWxString
extern KX_API const wxString KxNullWxString;
#endif

// Libs
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Rpcrt4.lib")

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
