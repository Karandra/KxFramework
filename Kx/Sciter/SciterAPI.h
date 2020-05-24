#pragma once
#include <sciter/sciter-x.h>
#include <sciter/sciter-x-api.h>
#include <sciter/sciter-x-behavior.h>

namespace kxf::Sciter
{
	extern void* g_SciterLibrary;

	ISciterAPI* GetSciterAPI();
	SciterGraphicsAPI* GetGrapchicsAPI();
	SciterRequestAPI* GetRequestAPI();
	tiscript_native_interface* GetSciptingAPI();
}
