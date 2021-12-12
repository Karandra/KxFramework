#pragma once
#include <sciter-js/sciter-x.h>
#include <sciter-js/sciter-x-api.h>
#include <sciter-js/sciter-x-behavior.h>

namespace kxf::Sciter
{
	ISciterAPI* GetSciterAPI();
	SciterGraphicsAPI* GetGrapchicsAPI();
	SciterRequestAPI* GetRequestAPI();
}
