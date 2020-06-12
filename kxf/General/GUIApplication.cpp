#include "stdafx.h"
#include "GUIApplication.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/System/SystemAppearance.h"
#include <wx/vidmode.h>
#include <uxtheme.h>

namespace kxf
{
	GUIApplication::GUIApplication()
	{
		wxInitAllImageHandlers();
	}

	System::DisplayInfo GUIApplication::GetDisplayMode() const
	{
		const wxVideoMode wxInfo = GetImpl().wxApp::GetDisplayMode();

		System::DisplayInfo displayInfo = {};
		displayInfo.Width = wxInfo.GetWidth();
		displayInfo.Height = wxInfo.GetHeight();
		displayInfo.BitDepth = wxInfo.GetDepth();
		displayInfo.RefreshRate = wxInfo.GetRefresh();

		return displayInfo;
	}
	UI::LayoutDirection GUIApplication::GetLayoutDirection() const
	{
		switch (GetImpl().wxApp::GetLayoutDirection())
		{
			case wxLayoutDirection::wxLayout_LeftToRight:
			{
				return UI::LayoutDirection::LeftToRight;
			}
			case wxLayoutDirection::wxLayout_RightToLeft:
			{
				return UI::LayoutDirection::RightToLeft;
			}
		};
		return UI::LayoutDirection::Default;
	}

	String GUIApplication::GetNativeTheme() const
	{
		if (System::IsWindows10OrGreater())
		{
			return SystemAppearance::IsDark() ? wxS("ModernUI Dark") : wxS("ModernUI Light");
		}
		else if (System::IsWindows8OrGreater())
		{
			return wxS("ModernUI");
		}
		else if (System::IsWindowsVistaOrGreater())
		{
			return ::IsAppThemed() ? wxS("Aero") : wxS("Classic");
		}
		else if (System::IsWindowsXPOrGreater())
		{
			return ::IsAppThemed() ? wxS("Luna") : wxS("Classic");
		}
		return {};
	}
	bool GUIApplication::SetNativeTheme(const String& themeName)
	{
		return GetImpl().wxApp::SetNativeTheme(themeName);
	}
}
