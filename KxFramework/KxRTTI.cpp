#include "KxStdAfx.h"
#include "KxFramework/KxRTTI.h"
#include <atomic>

namespace
{
	std::atomic<size_t> ms_TypeID = 1;
}

size_t KxRTTI::NewTypeID()
{
	return ms_TypeID.fetch_add(1);
}
