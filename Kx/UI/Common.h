#pragma once
#include "Kx/Common.hpp"
class wxWindow;

namespace KxFramework::UI
{
	// Get any valid HWND for functions that requires parent window in all cases.
	// Return HWND of windows in following chain:
	// window -> wxTheApp->GetTopWindow() -> wxGetTopLevelParent(wxGetActiveWindow()) -> ::GetShellWindow()
	void* GetOwnerWindowHandle(const wxWindow* window) noexcept;
}
