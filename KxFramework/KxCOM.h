/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxCOMInit final
{
	private:
		static HRESULT GetInvalidHRESULT()
		{
			return CO_E_NOTINITIALIZED;
		}

	private:
		HRESULT m_Result = GetInvalidHRESULT();

	public:
		KxCOMInit(tagCOINIT options = tagCOINIT::COINIT_APARTMENTTHREADED);
		KxCOMInit(const KxCOMInit&) = delete;
		KxCOMInit(KxCOMInit&& other)
		{
			*this = std::move(other);
		}
		~KxCOMInit();

	public:
		HRESULT GetResult() const
		{
			return m_Result;
		}
		bool IsInitialized() const
		{
			return SUCCEEDED(m_Result);
		}
		void Uninitialize();

		operator bool() const
		{
			return IsInitialized();
		}
		bool operator!() const
		{
			return !IsInitialized();
		}

	public:
		KxCOMInit& operator=(const KxCOMInit&) = delete;
		KxCOMInit& operator=(KxCOMInit&& other)
		{
			m_Result = other.m_Result;
			other.m_Result = GetInvalidHRESULT();

			return *this;
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxOLEInit final
{
	private:
		static HRESULT GetInvalidHRESULT()
		{
			return CO_E_NOTINITIALIZED;
		}

	private:
		HRESULT m_Result = GetInvalidHRESULT();

	public:
		KxOLEInit();
		KxOLEInit(const KxCOMInit&) = delete;
		KxOLEInit(KxOLEInit&& other)
		{
			*this = std::move(other);
		}
		~KxOLEInit();

	public:
		HRESULT GetResult() const
		{
			return m_Result;
		}
		bool IsInitialized() const
		{
			return SUCCEEDED(m_Result);
		}
		void Uninitialize();

		operator bool() const
		{
			return IsInitialized();
		}
		bool operator!() const
		{
			return !IsInitialized();
		}

	public:
		KxOLEInit& operator=(const KxOLEInit&) = delete;
		KxOLEInit& operator=(KxOLEInit&& other)
		{
			m_Result = other.m_Result;
			other.m_Result = GetInvalidHRESULT();

			return *this;
		}
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KxCOMPtr final
{
	private:
		T* m_Ptr = nullptr;

	public:
		KxCOMPtr(T* ptr = nullptr)
			:m_Ptr(ptr)
		{
			static_assert(std::is_base_of<IUnknown, T>::value, "class T is not derived from IUnknown");
		}
		KxCOMPtr(KxCOMPtr&& other)
		{
			*this = std::move(other);
		}
		~KxCOMPtr()
		{
			Reset();
		}

	public:
		void Reset(T* newPtr = nullptr)
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
			m_Ptr = nullptr;
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
		
		operator const T*() const
		{
			return m_Ptr;
		}
		operator T* ()
		{
			return m_Ptr;
		}

		const T& operator*() const
		{
			return *m_Ptr;
		}
		T& operator*()
		{
			return *m_Ptr;
		}

		T** operator&()
		{
			return &m_Ptr;
		}

		explicit operator bool() const
		{
			return m_Ptr != nullptr;
		}
		bool operator!() const
		{
			return m_Ptr == nullptr;
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
		KxCOMPtr& operator=(KxCOMPtr&& other)
		{
			Reset(other.Detach());
			return *this;
		}
		KxCOMPtr& operator=(T* ptr)
		{
			Reset(ptr);
			return *this;
		}
};
