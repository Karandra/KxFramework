#pragma once

#ifdef KXF_DYNAMIC_LIBRARY
	#ifdef KXF_LIBRARY
		#define KX_API __declspec(dllexport)
	#else
		#define KX_API __declspec(dllimport)
	#endif
#elif defined KXF_STATIC_LIBRARY
	#ifdef KXF_LIBRARY
		#define KX_API __declspec(dllexport)
	#else
		#define KX_API
	#endif
#endif

// Standard library
#include <cstdint>
#include <memory>
#include <optional>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <tuple>

// Essential headers
#include "kxf/wxWidgets/Setup.h"
#include "kxf/General/FlagSet.h"
#include "kxf/General/StdID.h"
#include "kxf/System/UndefWindows.h"
#include "kxf/wxWidgets/Setup.h"

// Forward declarations
namespace kxf
{
	template<class TValue>
	class Enumerator;
}

// wxWidgets
// Link some libraries that aren't linked by wxWidgets itself for some reason

// Required by pretty much any UI component
#pragma comment(lib, "Comctl32.lib")

// Required by various GPU-related functions
#pragma comment(lib, "dxgi.lib")

// Required by network module
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "DNSAPI.lib")
#pragma comment(lib, "WinINet.lib")
#pragma comment(lib, "Wldap32.lib")

// Multimedia
#pragma comment(lib, "Winmm.lib")

// Required by UUID/GUID functions
#pragma comment(lib, "Rpcrt4.lib")

// Required by Crypto module
#pragma comment(lib, "Crypt32.lib")
