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
#include <type_traits>

// Essential headers
#include "Kx/wxWidgets/Setup.h"
#include "Kx/General/EnumClass.h"
#include "Kx/System/UndefWindows.h"

// wxWidgets
// Required by wxWidgets network module which doesn't link to this library itself for some reason
#pragma comment(lib, "WS2_32.lib")
