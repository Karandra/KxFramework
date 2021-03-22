#include "KxfPCH.h"
#include "Memory.h"

namespace kxf::Utility
{
	void SecureZeroMemory(void* ptr, size_t size)
	{
		::RtlSecureZeroMemory(ptr, size);
	}
}
