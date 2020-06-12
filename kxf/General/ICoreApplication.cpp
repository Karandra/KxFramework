#include "stdafx.h"
#include "ICoreApplication.h"

namespace
{
	std::atomic<kxf::ICoreApplication*> g_AppInstance = nullptr;
}

namespace kxf
{
	ICoreApplication* ICoreApplication::GetInstance() noexcept
	{
		return g_AppInstance;
	}
	void ICoreApplication::SetInstance(ICoreApplication* instance) noexcept
	{
		g_AppInstance = instance;
	}
}
