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

// Main library includes
#include <KxFramework/KxStandardID.h>
#include <KxFramework/KxButtonType.h>
#include <KxFramework/KxIconType.h>
#include "KxFramework/KxMath.h"
