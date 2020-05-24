#pragma once
#include "Common.h"
#include <wx/buffer.h>

namespace kxf
{
	template<class T>
	class KX_API MemorySpan final
	{
		public:
			using ValueType = T;

		private:
			ValueType* m_Data = nullptr;
			size_t m_Size = 0;

		public:
			MemorySpan() noexcept = default;
			MemorySpan(ValueType* data, size_t size) noexcept
				:m_Data(data), m_Size(size)
			{
			}
			MemorySpan(wxScopedCharTypeBuffer<ValueType>& buffer) noexcept
				:m_Data(buffer.data()), m_Size(buffer.length())
			{
			}
			MemorySpan(wxMemoryBuffer& buffer) noexcept
				:m_Data(buffer.GetData()), m_Size(buffer.GetDataLen())
			{
			}

		public:
			const ValueType* data() const noexcept
			{
				return m_Data;
			}
			ValueType* data() noexcept
			{
				return m_Data;
			}

			bool empty() const noexcept
			{
				return m_Data == nullptr || m_Size == 0;
			}
			size_t size() const noexcept
			{
				return m_Size;
			}

			ValueType* begin() noexcept
			{
				return m_Data;
			}
			const ValueType* begin() const noexcept
			{
				return m_Data;
			}

			ValueType* end() noexcept
			{
				return m_Data + m_Size;
			}
			const ValueType* end() const noexcept
			{
				return m_Data + m_Size;
			}
			
		public:
			explicit operator bool() const noexcept
			{
				return !empty();
			}
			bool operator!() const noexcept
			{
				return empty();
			}
	};

	using UntypedMemorySpan = MemorySpan<void>;
}
