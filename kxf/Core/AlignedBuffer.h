#pragma once
#include "Common.h"
#include "kxf/Utility/Memory.h"

namespace kxf
{
	template<class TValue_, size_t t_Size = sizeof(TValue_), size_t t_Alignment = alignof(TValue_)>
	class AlignedBuffer final
	{
		public:
			using TValue = typename std::conditional_t<std::is_reference_v<TValue_>, std::reference_wrapper<std::remove_reference_t<TValue_>>, TValue_>;

		private:
			alignas(t_Alignment) uint8_t m_Buffer[t_Size] = {};

		public:
			AlignedBuffer() noexcept = default;
			AlignedBuffer(const AlignedBuffer&) = delete;
			AlignedBuffer(AlignedBuffer&&) = delete;
			~AlignedBuffer() noexcept = default;

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

			template<class... Args, std::enable_if_t<std::is_constructible_v<TValue, Args...>, int> = 0>
			TValue& Construct(Args&&... arg) noexcept(std::is_nothrow_constructible_v<TValue, Args...>)
			{
				static_assert(t_Size >= sizeof(TValue), "insufficient buffer size");
				static_assert(t_Alignment == alignof(TValue), "alignment doesn't match");

				return *Utility::ConstructAt<TValue>(static_cast<void*>(m_Buffer), std::forward<Args>(arg)...);
			}

			void Destroy() noexcept(std::is_nothrow_destructible_v<TValue>)
			{
				Utility::DestroyAt<TValue>(data());
			}
			void ZeroBuffer() noexcept
			{
				std::memset(m_Buffer, 0, t_Size);
			}

		public:
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

			const TValue* operator->() const noexcept
			{
				return get();
			}
			TValue* operator->() noexcept
			{
				return get();
			}

			AlignedBuffer& operator=(const AlignedBuffer&) = delete;
			AlignedBuffer& operator=(AlignedBuffer&&) = delete;
	};
}
