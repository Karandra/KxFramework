#pragma once
#include "Common.h"
#include "IGUIApplication.h"
#include "CoreApplication.h"

namespace kxf
{
	class KX_API GUIApplication: public RTTI::ImplementInterface<GUIApplication, CoreApplication, IGUIApplication>
	{
		public:
			GUIApplication();

		public:
			System::DisplayInfo GetDisplayMode() const override;
			UI::LayoutDirection GetLayoutDirection() const override;

			bool IsActive() const override
			{
				return GetImpl().wxApp::IsActive();
			}
			wxWindow* GetTopWindow() const override
			{
				return GetImpl().wxApp::GetTopWindow();
			}
			void SetTopWindow(wxWindow* window) override
			{
				GetImpl().wxApp::SetTopWindow(window);
			}

			String GetNativeTheme() const override;
			bool SetNativeTheme(const String& themeName) override;
	};
}
