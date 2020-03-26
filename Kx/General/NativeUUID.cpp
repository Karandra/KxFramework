#include "KxStdAfx.h"
#include "NativeUUID.h"
#include "UUID.h"

namespace std
{
	size_t hash<KxFramework::NativeUUID>::operator()(const KxFramework::NativeUUID& uuid) const noexcept
	{
		return KxFramework::UUID(uuid).GetHash();
	}
}
