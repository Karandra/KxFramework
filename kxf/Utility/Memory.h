#pragma once
#include "kxf/Common.hpp"
#include <memory>
#include <new>
#include "kxf/System/UndefWindows.h"

namespace kxf::Utility
{
	void SecureZeroMemory(void* ptr, size_t size);

	template<class T, class TDealloc, class... Args>
	T* NewObjectOnMemoryLocation(void* buffer, TDealloc&& dealloc, Args&&... arg) noexcept(std::is_nothrow_constructible_v<T, Args...>)
	{
		auto DoConstruct = [&]()
		{
			return std::launder(new(buffer) T(std::forward<Args>(arg)...));
		};

		if constexpr(std::is_nothrow_constructible_v<T, Args...>)
		{
			return DoConstruct();
		}
		else
		{
			try
			{
				return *DoConstruct();
			}
			catch (...)
			{
				std::invoke(dealloc);
				throw;
			}
		}
	}

	template<class T>
	void DestroyObjectOnMemoryLocation(void* buffer) noexcept(std::is_nothrow_destructible_v<T>)
	{
		static_cast<T*>(buffer)->~T();
	}
}

namespace kxf::Utility
{
	template<class T>
	class AlignedHeapBuffer final
	{
		public:
			using ValueType = T;

		private:
			std::allocator<uint8_t> m_Allocator;

			void* m_Source = nullptr;
			void* m_Aligned = nullptr;
			size_t m_Size = 0;

		public:
			AlignedHeapBuffer(size_t size = alignof(T))
				:m_Size(size)
			{
				m_Source = m_Allocator.allocate(size + alignof(ValueType));

				void* toAlign = m_Source;
				m_Aligned = std::align(size, sizeof(T), toAlign, m_Size);
			}
			AlignedHeapBuffer(const AlignedHeapBuffer&) = delete;
			AlignedHeapBuffer(AlignedHeapBuffer&& other) noexcept
			{
				*this = std::move(other);
			}
			~AlignedHeapBuffer()
			{
				m_Allocator.deallocate(reinterpret_cast<uint8_t*>(m_Source), m_Size + alignof(ValueType));
			}

		public:
			void* GetSource() const noexcept
			{
				return m_Source;
			}
			void* GetAligned() const noexcept
			{
				return m_Aligned;
			}
			size_t GetSize() const noexcept
			{
				return m_Size;
			}

		public:
			explicit operator bool() const noexcept
			{
				return m_Aligned != nullptr;
			}
			bool operator!() const noexcept
			{
				return m_Aligned == nullptr;
			}

			AlignedHeapBuffer& operator=(const AlignedHeapBuffer&) = delete;
			AlignedHeapBuffer& operator=(AlignedHeapBuffer&& other) noexcept
			{
				m_Allocator = std::move(other.m_Allocator);

				m_Source = other.m_Source;
				other.m_Source = nullptr;

				m_Aligned = other.m_Aligned;
				other.m_Aligned = nullptr;

				m_Size = other.m_Size;
				other.m_Size = nullptr;

				return *this;
			}
	};
}
