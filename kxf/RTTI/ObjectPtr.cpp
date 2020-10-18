#include "stdafx.h"
#include "ObjectPtr.h"

namespace
{
	kxf::RTTI::StdObjectDeleter<std::default_delete<kxf::IObject>> g_DefaultDeleter;
}

namespace kxf::RTTI
{
	ObjectDeleter& GetDefaultDeleter() noexcept
	{
		return g_DefaultDeleter;
	}
}
