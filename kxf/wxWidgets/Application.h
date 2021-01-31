#pragma once
#include <wx/app.h>
#include "kxf/RTTI/RTTI.h"

namespace kxf::wxWidgets
{
	class KX_API ApplicationConsole: public RTTI::Interface<ApplicationConsole>, public wxAppConsole
	{
		KxRTTI_DeclareIID(ApplicationConsole, {0x912d2b7f, 0x51aa, 0x43e5, {0xa6, 0x0, 0xc4, 0xad, 0xb8, 0x59, 0xdf, 0x69}});

		public:
			virtual ~ApplicationConsole() = default;
	};

	class KX_API Application: public RTTI::Interface<Application>, public wxApp
	{
		KxRTTI_DeclareIID(Application, {0xa856ae88, 0x6783, 0x44b8, {0x96, 0x6c, 0xe8, 0x20, 0x32, 0x6, 0xd9, 0x94}});

		public:
			virtual ~Application() = default;
	};
}
