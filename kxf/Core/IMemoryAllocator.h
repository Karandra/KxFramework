#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	enum class MemoryAllocatorFlag: uint32_t
	{
		None = 0,

		ZeroMemory = FlagSetValue<MemoryAllocatorFlag>(0)
	};
}

namespace kxf
{
	class KX_API IMemoryAllocator: public RTTI::Interface<IMemoryAllocator>
	{
		KxRTTI_DeclareIID(IMemoryAllocator, {0xef410429, 0xfd79, 0x4ecc, {0xb4, 0xa7, 0xad, 0x8c, 0x39, 0xc5, 0xcb, 0x56}});

		public:
			struct AllocationInfo
			{
				size_t Size = 0;
				size_t Alignment = 0;
				FlagSet<MemoryAllocatorFlag> Flags;

				constexpr explicit operator bool() const noexcept
				{
					return Size != 0;
				}
				constexpr bool operator!() const noexcept
				{
					return Size == 0;
				}
			};

		public:
			virtual void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept = 0;
			virtual bool Free(void* ptr) noexcept = 0;
			virtual AllocationInfo QueryAllocationInfo(void* ptr) const noexcept = 0;

			virtual size_t GetRequestedBytes() const noexcept = 0;
			virtual size_t GetAllocatedBytes() const noexcept = 0;
	};

	class KX_API IMemoryPoolAllocator: public RTTI::Interface<IMemoryPoolAllocator>
	{
		KxRTTI_DeclareIID(IMemoryPoolAllocator, {0xa295e939, 0x101, 0x4224, {0xb3, 0x39, 0x78, 0x7a, 0xa7, 0x71, 0x27, 0xb3}});

		public:
			virtual void* AllocatePool(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept = 0;
			virtual void* AttachPool(void* buffer, size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept = 0;
			virtual void FreePool() noexcept = 0;
			virtual size_t GetPoolSize() const noexcept = 0;

			virtual size_t GetPageSize() const noexcept = 0;
			virtual bool SetPageSize(size_t size) noexcept = 0;
	};
}

namespace kxf
{
	template<class T>
	class StdMemoryAllocator final
	{
		template<class Tx>
		friend class StdMemoryAllocator;

		private:
			std::shared_ptr<IMemoryAllocator> m_Allocator;
			FlagSet<MemoryAllocatorFlag> m_AllocationFlags;

		public:
			using value_type = T;
			using size_type = size_t;
			using difference_type = ptrdiff_t;

		public:
			StdMemoryAllocator(std::shared_ptr<IMemoryAllocator> allocator, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept
				:m_Allocator(std::move(allocator)), m_AllocationFlags(flags)
			{
			}

			template<class Tx>
			StdMemoryAllocator(const StdMemoryAllocator<Tx>& other) noexcept
				:m_Allocator(other.m_Allocator), m_AllocationFlags(other.m_AllocationFlags)
			{
			}

		public:
			T* allocate(size_t count)
			{
				auto ptr = m_Allocator->Allocate(count * sizeof(T), 0, m_AllocationFlags);
				if (!ptr)
				{
					throw std::bad_alloc();
				}

				return static_cast<T*>(ptr);
			}
			void deallocate(T* ptr, size_t count)
			{
				if (!m_Allocator->Free(ptr))
				{
					throw std::bad_alloc();
				}
			}

		public:
			template<class Tx>
			StdMemoryAllocator& operator=(const StdMemoryAllocator<Tx>& other) noexcept
			{
				m_Allocator = other.m_Allocator;
				m_AllocationFlags = other.m_AllocationFlags;

				return *this;
			}
	};
}
