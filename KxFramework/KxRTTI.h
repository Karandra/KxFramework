#pragma once

namespace KxRTTI
{
	template<class BaseT> class DynamicCastAsIs
	{
		public:
			virtual ~DynamicCastAsIs() = default;

		public:
			template<class T> bool As(T*& ptr)
			{
				static_assert(std::is_base_of<BaseT, T>::value, "T must be derived from 'Base'");

				ptr = dynamic_cast<T*>(this);
				return ptr != nullptr;
			}
			template<class T> bool As(const T*& ptr) const
			{
				static_assert(std::is_base_of<BaseT, T>::value, "T must be derived from 'Base'");

				ptr = dynamic_cast<const T*>(this);
				return ptr != nullptr;
			}

			template<class T> bool Is() const
			{
				const T*& ptr = nullptr;
				return As<T>(ptr);
			}
	};

	//////////////////////////////////////////////////////////////////////////
	template<class BaseT> class StaticTypeID
	{
		public:
			virtual ~StaticTypeID() = default;

		public:
			template<class T> bool As(T*& ptr)
			{
				static_assert(std::is_base_of<BaseT, T>::value, "T must be derived from 'Base'");
				
				ptr = nullptr;
				if (BaseT::GetTypeID() == T::GetTypeID())
				{
					ptr = static_cast<T*>(this);
					return true;
				}
				return false;
			}
			template<class T> bool As(const T*& ptr) const
			{
				static_assert(std::is_base_of<BaseT, T>::value, "T must be derived from 'Base'");

				ptr = nullptr;
				if (BaseT::GetTypeID() == T::GetTypeID())
				{
					ptr = static_cast<const T*>(this);
					return true;
				}
				return false;
			}

			template<class T> bool Is() const
			{
				const T*& ptr = nullptr;
				return As<T>(ptr);
			}
	};

	//////////////////////////////////////////////////////////////////////////
	size_t NewTypeID();
}
