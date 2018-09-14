#pragma once
#include "KxFramework/KxFramework.h"

class KxCOMInit
{
	private:
		HRESULT m_Result = 0;
		bool m_IsInit = false;

	public:
		KxCOMInit(DWORD options);
		~KxCOMInit();

	public:
		HRESULT GetResult() const
		{
			return m_Result;
		}
		void Uninitialize();

		operator bool() const
		{
			return m_IsInit;
		}
		bool operator!() const
		{
			return !m_IsInit;
		}
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KxCOMPtr
{
	private:
		T* m_Ptr = NULL;

	public:
		KxCOMPtr(T* ptr = NULL)
			:m_Ptr(ptr)
		{
			static_assert(std::is_base_of<IUnknown, T>::value, "class T is not derived from IUnknown");
		}
		~KxCOMPtr()
		{
			if (m_Ptr)
			{
				m_Ptr->Release();
				m_Ptr = NULL;
			}
		}

	public:
		void Reset(T* newPtr = NULL)
		{
			if (m_Ptr)
			{
				m_Ptr->Release();
			}
			m_Ptr = newPtr;
		}
		T* Detach()
		{
			T* ptr = m_Ptr;
			m_Ptr = NULL;
			return ptr;
		}

		T* Get() const
		{
			return m_Ptr;
		}
		void** GetPVoid() const
		{
			return reinterpret_cast<void**>(const_cast<T**>(&m_Ptr));
		}
		
		operator T*()
		{
			return m_Ptr;
		}
		T** operator&()
		{
			return &m_Ptr;
		}

		operator bool() const
		{
			return m_Ptr != NULL;
		}
		bool operator!() const
		{
			return m_Ptr == NULL;
		}

		T* operator->() const
		{
			return m_Ptr;
		}
		T* operator->()
		{
			return m_Ptr;
		}

		KxCOMPtr& operator=(const KxCOMPtr&) = delete;
		KxCOMPtr& operator=(T* ptr)
		{
			Reset(ptr);
			return *this;
		}
};
