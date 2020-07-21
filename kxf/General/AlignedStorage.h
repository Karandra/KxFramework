#pragma once
#include "Common.h"
#include "kxf/Utility/Memory.h"
#include <new>

namespace kxf
{
	template<class T, size_t t_Size, size_t t_Alignment>
	class AlignedBuffer final
	{
		public:
			using TValue = T;

		private:
			alignas(t_Alignment) uint8_t m_Buffer[t_Size] = {};

		public:
			AlignedBuffer() noexcept = default;
			AlignedBuffer(const AlignedBuffer&) = delete;
			AlignedBuffer(AlignedBuffer&&) = delete;

		public:
			const void* data() const noexcept
			{
				return static_cast<const void*>(m_Buffer);
			}
			void* data() noexcept
			{
				return static_cast<void*>(m_Buffer);
			}

			size_t size() const noexcept
			{
				return t_Size;
			}
			size_t alignment() const noexcept
			{
				return t_Alignment;
			}

			const TValue* get() const noexcept
			{
				return static_cast<const TValue*>(data());
			}
			TValue* get() noexcept
			{
				return static_cast<TValue*>(data());
			}

			template<class... Args>
			TValue& Construct(Args&&... arg) noexcept(std::is_nothrow_constructible_v<TValue, Args...>)
			{
				static_assert(t_Size >= sizeof(TValue), "insufficient buffer size");
				static_assert(t_Alignment == alignof(TValue), "alignment doesn't match");

				return *Utility::NewObjectOnMemoryLocation<TValue>(static_cast<void*>(m_Buffer), [&]()
				{
					Destruct();
				}, std::forward<Args>(arg)...);
			}

			void Destruct() noexcept(std::is_nothrow_destructible_v<TValue>)
			{
				Utility::DestroyObjectOnMemoryLocation<TValue>(data());
			}
			void Zero() noexcept
			{
				std::memset(m_Buffer, 0, t_Size);
			}

		public:
			const T* operator&() const noexcept
			{
				return get();
			}
			T* operator&() noexcept
			{
				return get();
			}

			const T& operator*() const noexcept
			{
				return *get();
			}
			T& operator*() noexcept
			{
				return *get();
			}

			const T* operator->() const noexcept
			{
				return get();
			}
			T* operator->() noexcept
			{
				return get();
			}

			AlignedBuffer& operator=(const AlignedBuffer&) = delete;
			AlignedBuffer& operator=(AlignedBuffer&&) = delete;
	};
}

namespace kxf
{
	template<class T, size_t t_Size, size_t t_Alignment>
	class AlignedStorage final
	{
		public:
			using TValue = T;

		private:
			AlignedBuffer<TValue, t_Size, t_Alignment> m_Buffer;
			TValue* m_Value = nullptr;

		public:
			AlignedStorage() noexcept(std::is_nothrow_constructible_v<T>)
			{
				Construct();
			}

			template<class... Args>
			AlignedStorage(Args&&... arg) noexcept(std::is_nothrow_constructible_v<T, Args...>)
			{
				Construct(std::forward<Args>(arg)...);
			}

			AlignedStorage(const AlignedStorage& other) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
			{
				Construct(*other.m_Value);
			}
			AlignedStorage(AlignedStorage&& other) noexcept(std::is_nothrow_move_constructible_v<TValue>)
			{
				Construct(std::move(*other.m_Value));
			}
			~AlignedStorage() noexcept(noexcept(Destruct()))
			{
				Destruct();
			}

		public:
			template<class... Args>
			T& Construct(Args&&... arg) noexcept(std::is_nothrow_constructible_v<T, Args...>)
			{
				if (!m_Value)
				{
					m_Value = &m_Buffer.Construct(std::forward<Args>(arg)...);
				}
				return *m_Value;
			}
			
			void Destruct() noexcept(std::is_nothrow_destructible_v<TValue>)
			{
				if (m_Value)
				{
					m_Buffer.Destruct();
					m_Value = nullptr;
				}
			}
			void Zero() noexcept
			{
				if (m_Value)
				{
					m_Buffer.Zero();
					m_Value = nullptr;
				}
			}

			bool IsConstructed() const noexcept
			{
				return m_Value != nullptr;
			}

		public:
			const void* data() const noexcept
			{
				return m_Buffer.data();
			}
			void* data() noexcept
			{
				return m_Buffer.data();
			}
			
			size_t size() const noexcept
			{
				return m_Buffer.size();
			}
			size_t alignment() const noexcept
			{
				return m_Buffer.alignment();
			}

			const T* get() const noexcept
			{
				return m_Value;
			}
			T* get() noexcept
			{
				return m_Value;
			}

		public:
			const T* operator&() const noexcept
			{
				return get();
			}
			T* operator&() noexcept
			{
				return get();
			}

			const T& operator*() const noexcept
			{
				return *get();
			}
			T& operator*() noexcept
			{
				return *get();
			}

			const T* operator->() const noexcept
			{
				return get();
			}
			T* operator->() noexcept
			{
				return get();
			}

			AlignedStorage& operator=(const AlignedStorage& other) noexcept(std::is_nothrow_copy_assignable_v<TValue>)
			{
				*m_Value = *other.m_Value;
				return *this;
			}
			AlignedStorage& operator=(AlignedStorage&& other) noexcept(std::is_nothrow_move_assignable_v<TValue>)
			{
				*m_Value = std::move(*other.m_Value);
				return *this;
			}
	};

	template<class T>
	using AlignedStorageComplete = AlignedStorage<T, sizeof(T), alignof(T)>;
}
