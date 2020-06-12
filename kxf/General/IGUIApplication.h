#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/RTTI/QueryInterface.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/UI/Common.h"
#include <wx/window.h>

namespace kxf
{
	class KX_API IGUIApplication: public RTTI::Interface<IGUIApplication>
	{
		KxDeclareIID(IGUIApplication, {0x51a21443, 0xb8e5, 0x46cd, {0xac, 0x6, 0xd0, 0x78, 0x84, 0x6c, 0xf0, 0x9f}});

		public:
			virtual ~IGUIApplication() = default;

		public:
			virtual System::DisplayInfo GetDisplayMode() const = 0;
			virtual UI::LayoutDirection GetLayoutDirection() const = 0;

			virtual bool IsActive() const = 0;
			virtual wxWindow* GetTopWindow() const = 0;
			virtual void SetTopWindow(wxWindow* window) = 0;

			virtual String GetNativeTheme() const = 0;
			virtual bool SetNativeTheme(const String& themeName) = 0;
	};
}
