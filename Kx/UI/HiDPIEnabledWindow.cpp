#include "KxStdAfx.h"
#include "HiDPIEnabledWindow.h"
#include "Kx/System/NativeAPI.h"

namespace KxFramework::UI::Private
{
	bool HiDPIEnabledWindowBase::DoEnableNonClientDPIScaling(wxWindow& window, bool enable)
	{
		if (NativeAPI::User32::EnableNonClientDpiScaling)
		{
			return NativeAPI::User32::EnableNonClientDpiScaling(window.GetHandle());
		}
		return false;
	}
}
