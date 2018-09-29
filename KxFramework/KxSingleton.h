#pragma once
#include "KxFramework/KxFramework.h"

template<class T> class KxSingleton
{
	public:
		static T& GetInstance()
		{
			static T ms_Instance;
			return ms_Instance;
		}

	private:
		KxSingleton(const KxSingleton&) = delete;
		KxSingleton& operator=(const KxSingleton&) = delete;
		KxSingleton& operator=(const KxSingleton&&) = delete;

	protected:
		KxSingleton() = default;
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KxSingletonPtr
{
	private:
		static KxSingletonPtr<T>* ms_Instance;

	public:
		static bool HasInstance()
		{
			return ms_Instance != NULL;
		}
		static T* GetInstance()
		{
			return static_cast<T*>(ms_Instance);
		}

	private:
		KxSingletonPtr(const KxSingletonPtr&) = delete;
		KxSingletonPtr& operator=(const KxSingletonPtr&) = delete;
		KxSingletonPtr& operator=(const KxSingletonPtr&&) = delete;

	public:
		KxSingletonPtr()
		{
			if (!HasInstance())
			{
				ms_Instance = this;
			}
			else
			{
				throw std::runtime_error("KxSingletonPtr: Only one instance of " __FUNCTION__ " is allowed");
			}
		}
		virtual ~KxSingletonPtr()
		{
			ms_Instance = NULL;
		}
};

#define KxSingletonPtr_Define(T)	KxSingletonPtr<T>* KxSingletonPtr<T>::ms_Instance = NULL
