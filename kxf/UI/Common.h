#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/Color.h"
#include "kxf/General/String.h"
#include "kxf/General/DateTime.h"
#include "kxf/General/StdID.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/EventSystem/Common.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/UI/StdIcon.h"
#include "kxf/UI/StdButton.h"
#include <wx/window.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::UI
{
	enum class WindowStyle
	{
		None = 0,

		Transparent = wxTRANSPARENT_WINDOW,
		TabTraversal = wxTAB_TRAVERSAL,
		WantsChars = wxWANTS_CHARS,
		ClipChildren = wxCLIP_CHILDREN,
		FullRepaintOnResize = wxFULL_REPAINT_ON_RESIZE,
		NoFullRepaintOnResize = wxNO_FULL_REPAINT_ON_RESIZE,
		ScrollVertical = wxVSCROLL,
		ScrollHorizontal = wxHSCROLL,
		ScrollShowAlways = wxALWAYS_SHOW_SB,
	};
	enum class WindowExtraStyle
	{
		None = 0,

		BlockEvents = wxWS_EX_BLOCK_EVENTS,
		Transient = wxWS_EX_TRANSIENT,
		ContextHelp = wxWS_EX_CONTEXTHELP,
		ProcessIdle = wxWS_EX_PROCESS_IDLE,
		ProcessUIUpdates = wxWS_EX_PROCESS_UI_UPDATES,
	};
	enum class WindowBorder
	{
		None = 0,

		Default = wxBORDER_DEFAULT,
		Simple = wxBORDER_SIMPLE,
		Sunken = wxBORDER_SUNKEN,
		Raised = wxBORDER_RAISED,
		Static = wxBORDER_STATIC,
		Theme = wxBORDER_THEME,
	};
}
namespace kxf
{
	Kx_DeclareFlagSet(UI::WindowStyle);
	Kx_DeclareFlagSet(UI::WindowExtraStyle);
	Kx_DeclareFlagSet(UI::WindowBorder);
}

namespace kxf::UI
{
	// Get any valid HWND for functions that requires parent window in all cases.
	// Return HWND of windows in following chain:
	// window -> wxTheApp->GetTopWindow() -> wxGetTopLevelParent(wxGetActiveWindow()) -> ::GetShellWindow()
	void* GetOwnerWindowHandle(const wxWindow* window) noexcept;
}
