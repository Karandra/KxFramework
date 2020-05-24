#include "stdafx.h"
#include "Common.h"
#include <wx/window.h>

namespace kxf::UI
{
	void* GetOwnerWindowHandle(const wxWindow* window) noexcept
	{
		if (window)
		{
			if (window = wxGetTopLevelParent(const_cast<wxWindow*>(window)))
			{
				return window->GetHandle();
			}
			else if (window = wxTheApp->GetTopWindow())
			{
				return window->GetHandle();
			}
			else if (window = wxGetTopLevelParent(wxGetActiveWindow()))
			{
				return window->GetHandle();
			}
		}
		return ::GetShellWindow();
	}
}
