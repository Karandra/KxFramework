#include "KxStdAfx.h"
#include "KxFramework/KxDynamicCastAsIs.h"
#include <atomic>

namespace Kx::RTTI
{
	size_t NewTypeID()
	{
		static std::atomic<size_t> ms_TypeID = 1;
		return ms_TypeID.fetch_add(1);
	}
}
