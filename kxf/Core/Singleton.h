#pragma once
#include "Common.h"

namespace kxf
{
	template<class TInstance_>
	class Singleton
	{
		public:
			using TInstance = TInstance_;

		public:
			static TInstance_& GetInstance() noexcept(std::is_nothrow_default_constructible_v<TInstance_>)
			{
				static TInstance_ instance;
				return instance;
			}

		protected:
			Singleton() noexcept = default;
			~Singleton() noexcept = default;

		public:
			Singleton(const Singleton&) = delete;
			Singleton(Singleton&&) = delete;

		public:
			Singleton& operator=(const Singleton&) = delete;
			Singleton& operator=(Singleton&&) = delete;
	};
}

namespace kxf
{
	template<class TInstance_>
	class SingletonPtr
	{
		public:
			using TInstance = TInstance_;

		private:
			static inline TInstance_* ms_Instance = nullptr;

		public:
			static TInstance_* GetInstance() noexcept
			{
				return ms_Instance;
			}

		public:
			SingletonPtr()
			{
				if (!ms_Instance)
				{
					ms_Instance = static_cast<TInstance_*>(this);
				}
				else
				{
					throw std::runtime_error("kxf::SingletonPtr: Only one instance of " __FUNCTION__ " is allowed");
				}
			}
			SingletonPtr(const SingletonPtr&) = delete;
			SingletonPtr(SingletonPtr&&) = delete;
			~SingletonPtr() noexcept
			{
				ms_Instance = nullptr;
			}
		
		public:
			SingletonPtr& operator=(const SingletonPtr&) = delete;
			SingletonPtr& operator=(SingletonPtr&&) = delete;
	};
}
