#pragma once

// SDK version
#include <SDKDDKVer.h>

// Standard library
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <variant>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <atomic>
#include <locale>
#include <exception>
#include <stdexcept>
#include <utility>
#include <compare>
#include <tuple>

// Often used wxWidgets headers

// Required because wxWidgets uses old CRT functions
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

// Since v3.1.4 wxWidgets is using this macro to switch class access modifiers
// depending on whether it's built as a static or dynamic library. We need to
// always define it as otherwise it's causing linker errors due to name mangling.
#ifndef WXBUILDING
#define WXBUILDING
#endif

#include "kxf/wxWidgets/Setup.h"
#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/dcgraph.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/renderer.h>
#include <wx/sysopt.h>
#include <wx/regex.h>
#include <wx/stream.h>
#include <wx/mstream.h>
#include <wx/datetime.h>

// Main kxf header
#include "kxf/Common.hpp"

// Undef any remaining Windows headers stuff
#include "kxf/System/UndefWindows.h"
