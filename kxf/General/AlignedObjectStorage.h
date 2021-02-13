#pragma once
#include "Common.h"
#include "AlignedStorage.h"

namespace kxf
{
	template<class TValue_, size_t t_Size = sizeof(TValue_), size_t t_Alignment = alignof(TValue_)>
	class AlignedObjectStorage final
	{
		public:
			using TStorage = AlignedStorage<TValue_, t_Size, t_Alignment>;
			using TBuffer = typename TStorage::TBuffer;
			using TValue = typename TBuffer::TValue;

		private:
			TStorage m_Storage;

		public:
			template<class... Args, std::enable_if_t<std::is_constructible_v<TValue, Args...>, int> = 0>
			AlignedObjectStorage(Args&&... arg) noexcept(std::is_nothrow_constructible_v<TValue, Args...>)
			{
				m_Storage.Construct(std::forward<Args>(arg)...);
			}

			template<std::enable_if_t<std::is_copy_assignable_v<TValue>, int> = 0>
			AlignedObjectStorage(const AlignedObjectStorage& other) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
			{
				m_Storage.CopyFrom(other.m_Storage);
			}

			template<std::enable_if_t<std::is_move_assignable_v<TValue>, int> = 0>
			AlignedObjectStorage(AlignedObjectStorage&& other) noexcept(std::is_nothrow_move_constructible_v<TValue>)
			{
				m_Storage.MoveFrom(std::move(other.m_Storage));
			}

			~AlignedObjectStorage() noexcept(std::is_nothrow_destructible_v<TValue>)
			{
				m_Storage.Destroy();
			}

		public:
			const TBuffer& GetBuffer() const
			{
				return m_Storage.GetBuffer();
			}
			TBuffer& GetBuffer()
			{
				return m_Storage.GetBuffer();
			}

			const TStorage& GetStorage() const
			{
				return m_Storage;
			}
			TStorage& GetStorage()
			{
				return m_Storage;
			}

			size_t size() const noexcept
			{
				return m_Storage.size();
			}
			size_t alignment() const noexcept
			{
				return m_Storage.alignment();
			}

			const TValue* get() const noexcept
			{
				return m_Storage.get();
			}
			TValue* get() noexcept
			{
				return m_Storage.get();
			}

			const TValue* operator&() const noexcept
			{
				return get();
			}
			TValue* operator&() noexcept
			{
				return get();
			}

			const TValue& operator*() const& noexcept
			{
				return *get();
			}
			TValue& operator*() & noexcept
			{
				return *get();
			}
			TValue operator*() && noexcept
			{
				return m_Storage.TakeValue();
			}

			const TValue* operator->() const noexcept
			{
				return get();
			}
			TValue* operator->() noexcept
			{
				return get();
			}

		public:
			template<std::enable_if_t<std::is_copy_assignable_v<TValue>, int> = 0>
			AlignedObjectStorage& operator=(const AlignedObjectStorage& other) noexcept(std::is_nothrow_constructible_v<TValue, const TValue&>)
			{
				m_Storage.CopyFrom(other.m_Storage);
				return *this;
			}

			template<std::enable_if_t<std::is_move_assignable_v<TValue>, int> = 0>
			AlignedObjectStorage& operator=(AlignedObjectStorage&& other) noexcept(std::is_nothrow_constructible_v<TValue, TValue&&>)
			{
				m_Storage.MoveFrom(std::move(other.m_Storage));
				return *this;
			}
	};
}
