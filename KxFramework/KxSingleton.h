#pragma once
#include "KxFramework/KxFramework.h"

template<class T> class KxSingleton
{
	public:
		using InstanceT = KxSingleton<T>;

	public:
		static T& GetInstance()
		{
			static T ms_Instance;
			return ms_Instance;
		}

	private:
		KxSingleton(const InstanceT&) = delete;
		InstanceT& operator=(const InstanceT&) = delete;

	protected:
		KxSingleton() = default;
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KxSingletonPtr
{
	public:
		using InstanceT = KxSingletonPtr<T>;

	private:
		static KxSingletonPtr* ms_Instance;

	public:
		static bool HasInstance()
		{
			return ms_Instance != NULL;
		}
		static T* GetInstance()
		{
			return static_cast<T*>(ms_Instance);
		}

	public:
		KxSingletonPtr()
		{
			if (!HasInstance())
			{
				ms_Instance = this;
			}
			else
			{
				delete this;
				throw std::runtime_error("KxSingletonPtr: Only one instance of " __FUNCTION__ " is allowed");
			}
		}
		virtual ~KxSingletonPtr()
		{
			ms_Instance = NULL;
		}
};

#define KxSingletonPtr_Define(T)	KxSingletonPtr<T>* KxSingletonPtr<T>::ms_Instance = NULL

//////////////////////////////////////////////////////////////////////////
template<class T> class KxSingletonUniquePtr
{
	public:
		using InstanceT = KxSingletonUniquePtr<T>;

	private:
		static std::unique_ptr<KxSingletonUniquePtr> ms_Instance;

	public:
		static bool HasInstance()
		{
			return ms_Instance.get() != NULL;
		}
		static T* GetInstance()
		{
			return static_cast<T*>(ms_Instance.get());
		}

	public:
		KxSingletonUniquePtr()
		{
			if (!HasInstance())
			{
				ms_Instance.reset(this);
			}
			else
			{
				delete this;
				throw std::runtime_error("KxSingletonUniquePtr: Only one instance of " __FUNCTION__ " is allowed");
			}
		}
};

#define KxSingletonUniquePtr_Define(T)	std::unique_ptr<KxSingletonUniquePtr> KxSingletonUniquePtr<T>::ms_Instance
