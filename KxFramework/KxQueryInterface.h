#pragma once
#include <typeinfo>

namespace Kx::RTTI
{
	using IID = std::size_t;

	template<class T> IID GetIIDOf() noexcept
	{
		return typeid(T).hash_code();
	}
}

namespace Kx::RTTI
{
	class IObject
	{
		public:
			using IID = RTTI::IID;

		protected:
			virtual bool OnQueryInterface(IObject*& object, const IID& iid) noexcept = 0;

		public:
			virtual ~IObject() = default;

		public:
			IObject* QueryInterface(const IID& uid) noexcept
			{
				IObject* object = nullptr;
				OnQueryInterface(object, uid);
				return object;
			}
			const IObject* QueryInterface(const IID& uid) const noexcept
			{
				IObject* object = nullptr;
				const_cast<IObject*>(this)->OnQueryInterface(object, uid);
				return object;
			}

			template<class T> T* QueryInterface() noexcept
			{
				return static_cast<T*>(QueryInterface(GetIIDOf<T>()));
			}
			template<class T> const T* QueryInterface() const noexcept
			{
				return static_cast<const T*>(QueryInterface(GetIIDOf<T>()));
			}

			template<class T> bool QueryInterface(T*& ptr) noexcept
			{
				ptr = QueryInterface<T>();
				return ptr != nullptr;
			}
			template<class T> bool QueryInterface(const T*& ptr) const noexcept
			{
				ptr = QueryInterface<T>();
				return ptr != nullptr;
			}
	};
}

namespace Kx::RTTI
{
	template<class I> class IInterface: public IObject
	{
		protected:
			bool OnQueryInterface(IObject*& object, const IID& iid) noexcept override
			{
				static const IID ms_IID = GetIIDOf<I>();
				if (iid == ms_IID)
				{
					object = static_cast<I*>(this);
					return true;
				}
				
				object = nullptr;
				return false;
			}
	};

	template<class I, class... BaseInterfaces> class IMultiInterface: public BaseInterfaces...
	{
		public:
			using IID = RTTI::IID;

		protected:
			bool OnQueryInterface(IObject*& object, const IID& iid) noexcept override
			{
				static const IID ms_IID = GetIIDOf<I>();
				if (iid == ms_IID)
				{
					object = static_cast<I*>(this);
					return true;
				}
				return (... || BaseInterfaces::OnQueryInterface(object, iid));
			}
	};
}

namespace Kx::RTTI
{
	namespace Internal
	{
		template<class T> class IObjectWrapper: public IObject {};
	}

	template<class... T> class IImplementation: public Internal::IObjectWrapper<IImplementation<T...>>, public T...
	{
		public:
			using IID = RTTI::IID;

		private:
			using Wrapper = Internal::IObjectWrapper<IImplementation<T...>>;

		protected:
			bool OnQueryInterface(IObject*& object, const IID& iid) noexcept override
			{
				static const IID ms_IID = GetIIDOf<IObject>();
				if (iid == ms_IID)
				{
					object = static_cast<Wrapper*>(this);
					return true;
				}
				return (T::OnQueryInterface(object, iid) || ...);
			}

		public:
			IObject* QueryInterface(const IID& uid) noexcept
			{
				IObject* object = nullptr;
				OnQueryInterface(object, uid);
				return object;
			}
			const IObject* QueryInterface(const IID& uid) const noexcept
			{
				IObject* object = nullptr;
				const_cast<IObject*>(this)->OnQueryInterface(object, uid);
				return object;
			}

			template<class T> T* QueryInterface() noexcept
			{
				if constexpr(std::is_same_v<T, IObject>)
				{
					return static_cast<Wrapper*>(this);
				}
				else
				{
					return static_cast<T*>(QueryInterface(GetIIDOf<T>()));
				}
			}
			template<class T> const T* QueryInterface() const noexcept
			{
				if constexpr(std::is_same_v<T, IObject>)
				{
					return static_cast<const Wrapper*>(this);
				}
				else
				{
					return static_cast<const T*>(QueryInterface(GetIIDOf<T>()));
				}
			}

			template<class T> bool QueryInterface(T*& ptr) noexcept
			{
				ptr = QueryInterface<T>();
				return ptr != nullptr;
			}
			template<class T> bool QueryInterface(const T*& ptr) const noexcept
			{
				ptr = QueryInterface<T>();
				return ptr != nullptr;
			}
	};
}
