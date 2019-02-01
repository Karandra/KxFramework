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
	template<class TInterface> class IInterface: public IObject
	{
		protected:
			bool OnQueryInterface(IObject*& object, const IID& iid) noexcept override
			{
				static const IID ms_IID = GetIIDOf<TInterface>();
				if (iid == ms_IID)
				{
					object = static_cast<TInterface*>(this);
					return true;
				}
				
				object = nullptr;
				return false;
			}
	};

	template<class TInterface, class... TBaseInterfaces> class IExtendInterface: public TBaseInterfaces...
	{
		public:
			using IID = RTTI::IID;

		protected:
			bool OnQueryInterface(IObject*& object, const IID& iid) noexcept override
			{
				static const IID ms_IID = GetIIDOf<TInterface>();
				if (iid == ms_IID)
				{
					object = static_cast<TInterface*>(this);
					return true;
				}
				return (... || TBaseInterfaces::OnQueryInterface(object, iid));
			}
	
		public:
			IExtendInterface() = default;
			template<class... Args> IExtendInterface(Args&&... arg)
				:TBaseInterfaces(std::forward<Args>(arg)...)...
			{
			}
	};
}

namespace Kx::RTTI
{
	namespace Internal
	{
		template<class T> class IObjectWrapper: public IObject {};
	}

	template<class... TInterface> class IImplementation: public Internal::IObjectWrapper<IImplementation<TInterface...>>, public TInterface...
	{
		public:
			using IID = RTTI::IID;

		private:
			using Wrapper = Internal::IObjectWrapper<IImplementation<TInterface...>>;

		protected:
			bool OnQueryInterface(IObject*& object, const IID& iid) noexcept override
			{
				static const IID ms_IID = GetIIDOf<IObject>();
				if (iid == ms_IID)
				{
					object = static_cast<Wrapper*>(this);
					return true;
				}
				return (TInterface::OnQueryInterface(object, iid) || ...);
			}

		public:
			IImplementation() = default;
			template<class... Args> IImplementation(Args&&... arg)
				:TInterface(std::forward<Args>(arg)...)...
			{
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
