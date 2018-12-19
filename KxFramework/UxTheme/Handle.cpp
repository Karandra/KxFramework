#include "KxStdAfx.h"
#include "Handle.h"
#include <Uxtheme.h>

namespace KxUxTheme
{
	Handle::Handle(const wxWindow* window, const wchar_t* classes)
	{
		m_Handle = ::OpenThemeData(window->GetHandle(), classes);
	}
	Handle::Handle(const wxWindow* window, const wchar_t* classes, uint32_t flags)
	{
		DWORD dwFlags = 0;
		if (flags & HandleFlags::ForceRectSizing)
		{
			dwFlags |= OTD_FORCE_RECT_SIZING;
		}
		if (flags & HandleFlags::NonClient)
		{
			dwFlags |= OTD_NONCLIENT;
		}

		m_Handle = ::OpenThemeDataEx(window->GetHandle(), classes, dwFlags);
	}

	Handle::~Handle()
	{
		if (m_Handle)
		{
			::CloseThemeData(m_Handle);
		}
	}
}
