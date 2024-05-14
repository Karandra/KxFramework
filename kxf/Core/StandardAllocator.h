#pragma once
#include "Common.h"
#include "IMemoryAllocator.h"

namespace kxf
{
	enum class StandardAllocatorKind
	{
		Unknown = -1,

		CRuntime,
		SystemHeap,
		SystemLocal,
		SystemGlobal,
		SystemVirtual,
		SystemCoTask
	};

	struct StandardAllocatorConfig
	{
	};
	struct SystemHeapAllocatorConfig: public StandardAllocatorConfig
	{
		void* Heap = nullptr;
	};
	struct SystemVirtualAllocatorConfig: public StandardAllocatorConfig
	{
		void* Process = nullptr;
		void* BaseAddress = nullptr;
		FlagSet<uint32_t> AllocationType = 0;
		FlagSet<uint32_t> DeallocationType = 0;
		FlagSet<uint32_t> PageProtection = 0;
	};
}

namespace kxf
{
	KX_API std::shared_ptr<IMemoryAllocator> GetStandardAllocator(StandardAllocatorKind kind, const StandardAllocatorConfig* config = nullptr);
}
