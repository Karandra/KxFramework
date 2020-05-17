#include "stdafx.h"
#include "Memory.h"

namespace KxFramework::Utility
{
	void SecureZeroMemory(void* ptr, size_t size)
	{
		::RtlSecureZeroMemory(ptr, size);
	}
}
