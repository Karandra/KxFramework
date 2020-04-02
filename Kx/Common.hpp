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
#include <optional>
#include <memory>

// Essential headers
#include "Kx/General/EnumClass.h"
#include "Kx/System/UndefWindows.h"
