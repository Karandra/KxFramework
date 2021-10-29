#include "KxfPCH.h"
#include "IObject.h"
#include "ClassInfo.h"
#include "kxf/General/String.h"
#include "kxf/General/StupidMemoryAllocator.h"

namespace
{
	std::array<uint8_t, 128> g_UnownedRefStorage;
	kxf::StupidMemoryAllocator g_UnownedRefAllocator;

	template<class T>
	class FixedAllocator final
	{
		template<class Tx>
		friend class FixedAllocator;

		public:
			using value_type = T;
			using size_type = size_t;
			using difference_type = ptrdiff_t;

		public:
			FixedAllocator() noexcept = default;

			template<class Tx>
			FixedAllocator(const FixedAllocator<Tx>& other) noexcept
			{
			}

			FixedAllocator(const FixedAllocator&) = default;

		public:
			T* allocate(size_t count)
			{
				g_UnownedRefAllocator.AttachPool(g_UnownedRefStorage.data(), g_UnownedRefStorage.size());

				auto ptr = g_UnownedRefAllocator.Allocate(count * sizeof(T), 0, m_AllocationFlags);
				if (!ptr)
				{
					throw std::bad_alloc();
				}

				return static_cast<T*>(ptr);
			}
			void deallocate(const T* ptr, size_t count)
			{
				if (!g_UnownedRefAllocator.Free(ptr))
				{
					throw std::bad_alloc();
				}
			}

		public:
			FixedAllocator& operator=(const FixedAllocator&) = default;
	};

	struct OwnerlessDeleter final
	{
		constexpr void operator()(kxf::IObject*) noexcept
		{
		}
	};

	std::shared_ptr<kxf::IObject> g_UnownedRef = {nullptr, OwnerlessDeleter(), FixedAllocator<kxf::IObject>()};
	kxf::RTTI::InterfaceClassInfo<kxf::IObject> g_ClassInfo;
}

namespace kxf
{
	const RTTI::ClassInfo& IObject::ms_ClassInfo = g_ClassInfo;
	std::shared_ptr<IObject>& IObject::ms_UnownedRef = g_UnownedRef;

	RTTI::QueryInfo IObject::DoQueryInterface(const IID& iid) noexcept
	{
		if (iid.IsOfType<IObject>())
		{
			return *this;
		}
		else if (iid.IsOfType<RTTI::ClassInfo>())
		{
			return static_cast<RTTI::ClassInfo&>(g_ClassInfo);
		}
		return nullptr;
	}
}
