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

#pragma comment(lib, "wxbase31u.lib")
#pragma comment(lib, "wxbase31u_net.lib")
#pragma comment(lib, "wxmsw31u_core.lib")
#pragma comment(lib, "wxmsw31u_adv.lib")
#pragma comment(lib, "wxmsw31u_aui.lib")
#pragma comment(lib, "wxmsw31u_webview.lib")
#pragma comment(lib, "wxmsw31u_stc.lib")
#pragma comment(lib, "wxmsw31u_html.lib")
#pragma comment(lib, "wxscintilla.lib")
#pragma comment(lib, "wxzlib.lib")
#pragma comment(lib, "wxpng.lib")
#pragma comment(lib, "wxjpeg.lib")
#pragma comment(lib, "wxtiff.lib")
#pragma comment(lib, "wxexpat.lib")
#pragma comment(lib, "wxregexu.lib")

// Main library includes
#include <KxFramework/KxStandardID.h>
#include <KxFramework/KxButtonType.h>
#include <KxFramework/KxIconType.h>
#include "KxFramework/KxString.h"
#include "KxFramework/KxColor.h"
#include "KxFramework/KxMath.h"
