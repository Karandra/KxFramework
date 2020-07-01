#pragma once

#if defined KXFRAMEWORK_LIBRARY
#define KX_API __declspec(dllexport)
#elif defined KXFRAMEWORK_PLUGIN
#define KX_API __declspec(dllimport)
#else
#define KX_API
#endif

// Standard library
#include <cstdint>
#include <utility>
#include <memory>
#include <optional>
#include <functional>
#include <type_traits>

// Essential headers
#include "kxf/wxWidgets/Setup.h"
#include "kxf/General/FlagSet.h"
#include "kxf/General/StdID.h"
#include "kxf/System/UndefWindows.h"
#include "kxf/wxWidgets/Setup.h"

// wxWidgets
// Link some libraries that aren't linked by wxWidgets itself for some reason

// Required by pretty much any UI component
#pragma comment(lib, "Comctl32.lib")

// Required by network module
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "DNSAPI.lib")
#pragma comment(lib, "WinINet.lib")
#pragma comment(lib, "Wldap32.lib")

// Required by UUID/GUID functions
#pragma comment(lib, "Rpcrt4.lib")

// Required by Crypto module
#pragma comment(lib, "Crypt32.lib")
