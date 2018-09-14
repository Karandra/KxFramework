#pragma once
#include "KxFramework/KxFramework.h"

template<class T> class KxUnownedBuffer
{
	public:
		using DataType = T;

	private:
		DataType* m_Data = NULL;
		size_t m_Size = 0;

	public:
		KxUnownedBuffer()
		{
		}
		KxUnownedBuffer(DataType* data, size_t size)
			:m_Data(data), m_Size(size)
		{
		}
		KxUnownedBuffer(wxMemoryBuffer& buffer)
			:m_Data(buffer.GetData()), m_Size(buffer.GetDataLen())
		{
		}
		KxUnownedBuffer(wxScopedCharTypeBuffer<DataType>& buffer)
			:m_Data(buffer.data()), m_Size(buffer.length())
		{
		}

	public:
		const DataType* data() const
		{
			return m_Data;
		}
		DataType* data()
		{
			return m_Data;
		}

		bool empty() const
		{
			return m_Data == NULL || m_Size == 0;
		}
		size_t size() const
		{
			return m_Size;
		}
		size_t length() const
		{
			return m_Size;
		}
};

//////////////////////////////////////////////////////////////////////////
using KxUnownedMemoryBuffer = KxUnownedBuffer<void>;
