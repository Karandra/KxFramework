/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

template<class T>
class KxSingleton
{
	public:
		using TObject = T;

	public:
		static TObject& GetInstance()
		{
			static TObject ms_Instance;
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
template<class T>
class KxSingletonPtr
{
	public:
		using TObject = T;

	private:
		static inline TObject* ms_Instance = nullptr;

	public:
		static TObject* GetInstance()
		{
			return ms_Instance;
		}

		template<class TFunc>
		static void IfHasInstance(TFunc&& func)
		{
			using TFuncResult = std::invoke_result_t<TFunc, TObject&>;
			static_assert(std::is_void_v<TFuncResult>, "TFunc should not return a value");

			if (ms_Instance)
			{
				std::invoke(func, *ms_Instance);
			}
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
				ms_Instance = static_cast<TObject*>(this);
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
