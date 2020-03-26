#include "KxStdAfx.h"
#include "QueryInterface.h"
#include "Kx/General/UUID.h"

namespace std
{
	size_t hash<KxFramework::IID>::operator()(const KxFramework::IID& iid) const noexcept
	{
		return KxFramework::UUID(iid.ToNativeUUID()).GetHash();
	}
}
