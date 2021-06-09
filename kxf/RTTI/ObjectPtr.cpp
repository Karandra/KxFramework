#include "KxfPCH.h"
#include "ObjectPtr.h"
#include "IObject.h"

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
