/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

template<class t_Allocator = std::allocator<uint8_t>>
class KxAlignedAllocator
{
	private:
		void* m_Source = nullptr;
		void* m_Aligned = nullptr;
		size_t m_Size = 0;

	public:
		KxAlignedAllocator(size_t size)
			:m_Size(size)
		{
			m_Source = t_Allocator().allocate(size);
		}
		~KxAlignedAllocator()
		{
			t_Allocator().deallocate(reinterpret_cast<uint8_t*>(m_Source), m_Size);
		}

	public:
		template<class T> T* Align(size_t bound = alignof(T))
		{
			m_Aligned = nullptr;
			if (std::align(bound, sizeof(T), m_Aligned, m_Size))
			{
				return reinterpret_cast<T*>(m_Source);
			}
			return nullptr;
		}

		void* GetSource() const
		{
			return m_Source;
		}
		void* GetAligned() const
		{
			return m_Aligned;
		}
		size_t GetSize() const
		{
			return m_Size;
		}
};
