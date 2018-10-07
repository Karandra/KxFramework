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
		KxSingleton(KxSingleton&&) = delete;
		KxSingleton& operator=(const KxSingleton&) = delete;
		KxSingleton& operator=(KxSingleton&&) = delete;

	protected:
		KxSingleton() = default;
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KxSingletonPtr
{
	private:
		static inline T* ms_Instance = NULL;

	public:
		static bool HasInstance()
		{
			return ms_Instance != NULL;
		}
		static T* GetInstance()
		{
			return ms_Instance;
		}

	private:
		KxSingletonPtr(const KxSingletonPtr&) = delete;
		KxSingletonPtr(KxSingletonPtr&&) = delete;
		KxSingletonPtr& operator=(const KxSingletonPtr&) = delete;
		KxSingletonPtr& operator=(KxSingletonPtr&&) = delete;

	public:
		KxSingletonPtr()
		{
			if (ms_Instance == NULL)
			{
				ms_Instance = static_cast<T*>(this);
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
