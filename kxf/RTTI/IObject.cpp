#include "stdafx.h"
#include "IObject.h"
#include "ClassInfo.h"
#include "kxf/General/String.h"

namespace kxf
{
	kxf::RTTI::InterfaceClassInfo<kxf::IObject> g_ClassInfo;
}

namespace kxf
{
	const RTTI::ClassInfo& IObject::ms_ClassInfo = g_ClassInfo;

	RTTI::QueryInfo IObject::DoQueryInterface(const IID& iid) noexcept
	{
		if (iid.IsOfType<IObject>())
		{
			return this;
		}
		else if (iid.IsOfType<RTTI::ClassInfo>())
		{
			return static_cast<RTTI::ClassInfo*>(&g_ClassInfo);
		}
		return nullptr;
	}
}
