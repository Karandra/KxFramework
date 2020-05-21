#include "stdafx.h"
#include "SystemAppearance.h"
#include <wx/settings.h>

namespace KxFramework::SystemAppearance
{
	String GetName()
	{
		return wxSystemSettings::GetAppearance().GetName();
	}
	bool IsDark()
	{
		return wxSystemSettings::GetAppearance().IsDark();
	}
	bool IsUsingDarkBackground()
	{
		return wxSystemSettings::GetAppearance().IsUsingDarkBackground();
	}
}
