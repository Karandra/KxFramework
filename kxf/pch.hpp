#pragma once
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

#include <SDKDDKVer.h>

// Standard library
#include <array>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <variant>
#include <atomic>
#include <locale>

// Often used wxWidgets headers
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

// Main KxFramework header
#include "kxf/Common.hpp"

#include "kxf/General/String.h"
#include "kxf/General/StringFormater.h"
#include "kxf/General/Color.h"
#include "kxf/General/Angle.h"
#include "kxf/General/DateTime.h"
#include "kxf/General/StdID.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/UI/StdButton.h"
#include "kxf/UI/StdIcon.h"

// Undef any remaining Windows headers stuff
#include "kxf/System/UndefWindows.h"