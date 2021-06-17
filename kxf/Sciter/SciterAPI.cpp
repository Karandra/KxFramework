#include "KxfPCH.h"
#include "SciterAPI.h"
#include "Common.h"

// Stylesheets
#include "Stylesheets/MasterStylesheetStorage.h"

// Widgets
#include "Widgets/TextBoxWidget.h"

// Native widgets
#include "Widgets/Native/TextBoxWidget.h"

namespace
{
	const constexpr std::wstring_view g_MasterStylesheet =
		#include "Stylesheets/MasterStylesheet.css"
		;
}

namespace kxf::Sciter::Private
{
	void RegisterAPI()
	{
		// Register master stylesheets
		MasterStylesheetStorage& stylesheetStorage = MasterStylesheetStorage::GetInstance();
		stylesheetStorage.AddItem(g_MasterStylesheet);

		// Register widgets
		TextBoxWidgetFactory::RegisterInstance();

		// Register native widgets
		NativeTextBoxWidgetFactory::RegisterInstance();

		// Apply global styles
		stylesheetStorage.ApplyGlobally();
	}
}

namespace kxf::Sciter
{
	ISciterAPI* GetSciterAPI()
	{
		static ISciterAPI* g_SciterAPI = nullptr;

		if (!g_SciterAPI)
		{
			if (!g_SciterLibrary)
			{
				// Try to load using default name to retain original Sciter usage pattern
				LoadLibrary("Sciter.dll");
			}

			if (g_SciterLibrary)
			{
				if (auto func = reinterpret_cast<SciterAPI_ptr>(::GetProcAddress(reinterpret_cast<HMODULE>(g_SciterLibrary), "SciterAPI")))
				{
					g_SciterAPI = func();
				}

				if (g_SciterAPI)
				{
					Private::RegisterAPI();
				}
			}
		}
		return g_SciterAPI;
	}
	SciterGraphicsAPI* GetGrapchicsAPI()
	{
		return GetSciterAPI()->GetSciterGraphicsAPI();
	}
	SciterRequestAPI* GetRequestAPI()
	{
		return GetSciterAPI()->GetSciterRequestAPI();
	}
}
