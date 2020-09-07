#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include <wx/window.h>

namespace kxf
{
	class KX_API IGUIApplication: public RTTI::Interface<IGUIApplication>
	{
		KxRTTI_DeclareIID(IGUIApplication, {0x9a8d298a, 0xbaa2, 0x4a7b, {0xb1, 0x83, 0x67, 0x8a, 0xf1, 0xfb, 0x0, 0x9e}});

		public:
			static IGUIApplication* GetInstance() noexcept
			{
				if (auto app = ICoreApplication::GetInstance())
				{
					return app->QueryInterface<IGUIApplication>();
				}
				return nullptr;
			}
			static void SetInstance(IGUIApplication* instance) noexcept
			{
				ICoreApplication::SetInstance(instance ? instance->QueryInterface<ICoreApplication>() : nullptr);
			}

		public:
			virtual ~IGUIApplication() = default;

		public:
			virtual wxWindow* GetTopWindow() const = 0;
			virtual void SetTopWindow(wxWindow* window) = 0;

			virtual bool ShoudExitOnLastFrameDelete() const = 0;
			virtual void ExitOnLastFrameDelete(bool enable = true) = 0;
			
			virtual bool IsActive() const = 0;
			virtual void SetActive(bool active = true, wxWindow* window = nullptr) = 0;

			virtual UI::LayoutDirection GetLayoutDirection() const = 0;
			virtual void SetLayoutDirection(UI::LayoutDirection direction) = 0;

			virtual String GetNativeTheme() const = 0;
			virtual bool SetNativeTheme(const String& themeName) = 0;

			virtual bool Yield(wxWindow& window, FlagSet<EventYieldFlag> flags) = 0;
			virtual bool YieldFor(wxWindow& window, FlagSet<EventCategory> toProcess) = 0;
	};
}
