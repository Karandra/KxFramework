#include "KxfPCH.h"
#include "HiDPIEnabledWindow.h"
#include "kxf/System/NativeAPI.h"

namespace kxf::UI::Private
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
