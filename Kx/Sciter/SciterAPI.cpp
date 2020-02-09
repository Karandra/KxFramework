#include "KxStdAfx.h"
#include "SciterAPI.h"
#include "Common.h"
#include <KxFramework/KxLibrary.h>

namespace KxSciter
{
	ISciterAPI* GetSciterAPI()
	{
		static ISciterAPI* g_SciterAPI = nullptr;

		if (!g_SciterAPI)
		{
			if (!g_SciterLibrary)
			{
				// Try to load using default name to retain original Sciter usage pattern
				LoadLibrary(wxS("Sciter.dll"));
			}

			if (g_SciterLibrary)
			{
				if (auto func = reinterpret_cast<SciterAPI_ptr>(::GetProcAddress(reinterpret_cast<HMODULE>(g_SciterLibrary), "SciterAPI")))
				{
					g_SciterAPI = func();
					if (g_SciterAPI)
					{
						tiscript::ni(g_SciterAPI->TIScriptAPI());
					}
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
	tiscript_native_interface* GetSciptingAPI()
	{
		return GetSciterAPI()->TIScriptAPI();
	}
}
