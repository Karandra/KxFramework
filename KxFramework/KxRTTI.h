#pragma once

namespace KxRTTI
{
	size_t NewTypeID();
	//////////////////////////////////////////////////////////////////////////

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
			static size_t GetTypeID()
			{
				static inline size_t ms_TypeID = NewTypeID();
				return ms_TypeID;
			}

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
	class IVirtualTypeID
	{
		private:
			virtual size_t DoGetTypeID() const = 0;

		public:
			virtual ~IVirtualTypeID() = default;

		public:
			size_t GetTypeID() const
			{
				return DoGetTypeID();
			}
	};

	template<class BaseT> class VirtualTypeID: public IVirtualTypeID
	{
		private:
			static inline size_t ms_TypeID = NewTypeID();

		private:
			virtual size_t DoGetTypeID() const override
			{
				return ms_TypeID;
			}

		public:
			template<class T> bool As(T*& ptr)
			{
				static_assert(std::is_base_of<BaseT, T>::value, "T must be derived from 'Base'");

				ptr = nullptr;
				if (DoGetTypeID() == T::ms_TypeID)
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
				if (DoGetTypeID() == T::ms_TypeID)
				{
					ptr = static_cast<T*>(this);
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
}
