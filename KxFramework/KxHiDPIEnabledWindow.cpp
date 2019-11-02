#include "KxStdAfx.h"
#include "KxFramework/KxHiDPIEnabledWindow.h"
#include "KxFramework/KxSystemAPI.h"

bool KxHiDPIEnabledWindowBase::DoEnableNonClientDPIScaling(wxWindow& window, bool enable)
{
	if (KxSystemAPI::EnableNonClientDpiScaling)
	{
		return KxSystemAPI::EnableNonClientDpiScaling(window.GetHandle());
	}
	return false;
}
