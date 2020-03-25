#pragma once
#include "Kx/Sciter/SciterAPI/sciter-x.h"
#include "Kx/Sciter/SciterAPI/sciter-x-api.h"
#include "Kx/Sciter/SciterAPI/sciter-x-behavior.h"

namespace KxFramework::Sciter
{
	extern void* g_SciterLibrary;

	ISciterAPI* GetSciterAPI();
	SciterGraphicsAPI* GetGrapchicsAPI();
	SciterRequestAPI* GetRequestAPI();
	tiscript_native_interface* GetSciptingAPI();
}
