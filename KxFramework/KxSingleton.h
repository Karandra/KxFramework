/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
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

	public:
		KxSingleton(const KxSingleton&) = delete;
		KxSingleton(KxSingleton&&) = delete;
		KxSingleton& operator=(const KxSingleton&) = delete;
		KxSingleton& operator=(KxSingleton&&) = delete;

	protected:
		KxSingleton() = default;
		~KxSingleton() = default;
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KxSingletonPtr
{
	private:
		static inline T* ms_Instance = nullptr;

	public:
		static bool HasInstance()
		{
			return ms_Instance != nullptr;
		}
		static T* GetInstance()
		{
			return ms_Instance;
		}

	public:
		KxSingletonPtr(const KxSingletonPtr&) = delete;
		KxSingletonPtr(KxSingletonPtr&&) = delete;
		KxSingletonPtr& operator=(const KxSingletonPtr&) = delete;
		KxSingletonPtr& operator=(KxSingletonPtr&&) = delete;

	public:
		KxSingletonPtr()
		{
			if (ms_Instance == nullptr)
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
			ms_Instance = nullptr;
		}
};
