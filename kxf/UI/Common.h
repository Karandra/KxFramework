#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/DateTime.h"
#include "kxf/General/StdID.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/Angle.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/Drawing/Font.h"
#include "kxf/Drawing/GDIRenderer/GDIFont.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/EventSystem/Common.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/UI/StdIcon.h"
#include "kxf/UI/StdButton.h"
#include <wx/window.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::UI
{
	enum class WindowStyle: uint32_t
	{
		None = 0,

		Transparent = wxTRANSPARENT_WINDOW,
		TabTraversal = wxTAB_TRAVERSAL,
		WantsChars = wxWANTS_CHARS,
		ClipChildren = wxCLIP_CHILDREN,
		FullRepaintOnResize = wxFULL_REPAINT_ON_RESIZE,
		NoFullRepaintOnResize = wxNO_FULL_REPAINT_ON_RESIZE,
		ScrollVertical = static_cast<uint32_t>(wxVSCROLL),
		ScrollHorizontal = static_cast<uint32_t>(wxHSCROLL),
		ScrollShowAlways = wxALWAYS_SHOW_SB,
	};
	enum class WindowExStyle: uint32_t
	{
		None = 0,

		Transient = wxWS_EX_TRANSIENT,
		BlockEvents = wxWS_EX_BLOCK_EVENTS,
		ContextHelp = wxWS_EX_CONTEXTHELP,
		ProcessIdle = wxWS_EX_PROCESS_IDLE,
		ProcessUIUpdates = wxWS_EX_PROCESS_UI_UPDATES,
	};
	enum class WindowBorder: uint32_t
	{
		None = 0,

		Default = wxBORDER_DEFAULT,
		Simple = wxBORDER_SIMPLE,
		Sunken = wxBORDER_SUNKEN,
		Raised = wxBORDER_RAISED,
		Static = wxBORDER_STATIC,
		Theme = wxBORDER_THEME,
	};
	enum class LayoutDirection
	{
		Default = wxLayoutDirection::wxLayout_Default,
		LeftToRight = wxLayoutDirection::wxLayout_LeftToRight,
		RightToLeft = wxLayoutDirection::wxLayout_RightToLeft,
	};
}
namespace kxf
{
	KxFlagSet_Declare(UI::WindowStyle);
	KxFlagSet_Declare(UI::WindowExStyle);
	KxFlagSet_Declare(UI::WindowBorder);
}

namespace kxf::UI
{
	// Get any valid HWND for functions that requires parent window in all cases.
	// Return HWND of windows in following chain:
	// window -> wxTheApp->GetTopWindow() -> wxGetTopLevelParent(wxGetActiveWindow()) -> ::GetShellWindow()
	void* GetOwnerWindowHandle(const wxWindow* window) noexcept;
}
