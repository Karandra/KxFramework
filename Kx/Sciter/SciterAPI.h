#pragma once
#include "Kx/Sciter/SciterAPI/sciter-x.h"
#include "Kx/Sciter/SciterAPI/sciter-x-api.h"

namespace KxSciter
{
	extern void* g_SciterLibrary;

	ISciterAPI* GetSciterAPI();
	SciterGraphicsAPI* GetSciterGrapchicsAPI();
	SciterRequestAPI* GetSciterRequestAPI();
	tiscript_native_interface* GetSciterSciptingAPI();
}
