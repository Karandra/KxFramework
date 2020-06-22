#pragma once
#include "Common.h"

namespace kxf
{
	template<class T>
	class Singleton
	{
		public:
			using TObject = T;

		public:
			static T& GetInstance()
			{
				static T instance;
				return instance;
			}

		protected:
			Singleton() = default;
			~Singleton() = default;

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
	template<class T>
	class SingletonPtr
	{
		public:
			using TObject = T;

		private:
			static inline T* ms_Instance = nullptr;

		public:
			static T* GetInstance()
			{
				return ms_Instance;
			}

		public:
			SingletonPtr()
			{
				if (!ms_Instance)
				{
					ms_Instance = static_cast<T*>(this);
				}
				else
				{
					throw std::runtime_error("KxSingletonPtr: Only one instance of " __FUNCTION__ " is allowed");
				}
			}
			SingletonPtr(const SingletonPtr&) = delete;
			SingletonPtr(SingletonPtr&&) = delete;
			virtual ~SingletonPtr()
			{
				ms_Instance = nullptr;
			}
		
		public:
			SingletonPtr& operator=(const SingletonPtr&) = delete;
			SingletonPtr& operator=(SingletonPtr&&) = delete;
	};
}
