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
#include "Kx/wxWidgets/Setup.h"
#include "Kx/General/FlagSet.h"
#include "Kx/General/StdID.h"
#include "Kx/System/UndefWindows.h"
#include "Kx/wxWidgets/Setup.h"

// wxWidgets
// Link some libraries that aren't linked by wxWidgets itself for some reason

// Required by pretty much any UI component
#pragma comment(lib, "Comctl32.lib")

// Required by network module
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "DNSAPI.lib")
#pragma comment(lib, "WinINet.lib")

// Required by UUID/GUID functions
#pragma comment(lib, "Rpcrt4.lib")

// Required by Crypto module
#pragma comment(lib, "Crypt32.lib")
