#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"

// This class takes 'const void*' instead of just 'void*'
// and return 'void*' by design.
class KxDataViewItem
{
	public:
		using Vector = std::vector<KxDataViewItem>;

	private:
		void* m_Value = nullptr;

	public:
		KxDataViewItem(const void* value = nullptr)
			:m_Value(const_cast<void*>(value))
		{
		}
		KxDataViewItem(intptr_t value)
			:m_Value(reinterpret_cast<void*>(value))
		{
		}
		explicit KxDataViewItem(size_t value)
			:m_Value(reinterpret_cast<void*>(value))
		{
		}

	public:
		bool IsOK() const
		{
			return m_Value != nullptr;
		}
		bool IsTreeRootItem() const
		{
			return !IsOK();
		}
		void MakeNull()
		{
			m_Value = nullptr;
		}
		
		void* GetValue() const
		{
			return m_Value;
		}
		intptr_t GetValueInt() const
		{
			return reinterpret_cast<intptr_t>(m_Value);
		}
		size_t GetValueUInt() const
		{
			return reinterpret_cast<size_t>(m_Value);
		}
		template<class T = void> T* GetValuePtr() const
		{
			return static_cast<T*>(GetValue());
		}

	public:
		bool operator==(const KxDataViewItem& other) const
		{
			return m_Value == other.m_Value;
		}
		bool operator!=(const KxDataViewItem& other) const
		{
			return !(*this == other);
		}
		
		operator bool() const
		{
			return IsOK();
		}
		bool operator!() const
		{
			return !IsOK();
		}
};

template<class T> class KxDataViewItemT: public KxDataViewItem
{
	public:
		KxDataViewItemT(const T* value = nullptr)
			:KxDataViewItem(value)
		{
		}

	public:
		const T* operator->() const
		{
			return static_cast<const T*>(GetValue());
		}
		T* operator->()
		{
			return static_cast<T*>(GetValue());
		}
};
