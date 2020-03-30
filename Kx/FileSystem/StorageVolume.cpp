#include "KxStdAfx.h"
#include "StorageVolume.h"
#include "LegacyVolume.h"
#include "FSPath.h"

namespace KxFramework
{
	size_t StorageVolume::Enumerate(std::function<bool(StorageVolume)> func)
	{
		return 0;
	}

	bool StorageVolume::IsValid() const
	{
		return !m_GUID.IsNull();
	}
	bool StorageVolume::DoesExist() const
	{
		return false;
	}
}
