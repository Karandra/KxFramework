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
		static T*& GetInstanceStore()
		{
			static T* ms_Instance = NULL;
			return ms_Instance;
		}

	public:
		static bool HasInstance()
		{
			return GetInstanceStore() != NULL;
		}
		static T* GetInstance()
		{
			return GetInstanceStore();
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
				GetInstanceStore() = static_cast<T*>(this);
			}
			else
			{
				throw std::runtime_error("KxSingletonPtr: Only one instance of " __FUNCTION__ " is allowed");
			}
		}
		virtual ~KxSingletonPtr()
		{
			GetInstanceStore() = NULL;
		}
};
