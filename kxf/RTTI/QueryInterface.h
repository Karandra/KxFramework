#pragma once
#include "Common.h"
#include "IID.h"
#include "ObjectPtr.h"
#include <kxf/Utility/Common.h>
#include <kxf/Utility/TypeTraits.h>

#define KxRTTI_DeclareIID(T, ...)	\
friend class kxf::IObject;	\
\
template<class T>	\
friend constexpr kxf::IID kxf::RTTI::GetInterfaceID() noexcept;	\
\
private:	\
	static constexpr kxf::IID ms_IID = kxf::NativeUUID __VA_ARGS__;

#define KxRTTI_DeclareIID_External(T, ...)	\
namespace RTTI	\
{	\
	template<>	\
	constexpr IID GetInterfaceID<T>() noexcept	\
	{	\
		return kxf::NativeUUID __VA_ARGS__;	\
	}	\
}

#define KxRTTI_QueryInterface_Base(T)	\
\
public:	\
void* DoQueryInterface(const kxf::IID& iid) noexcept override	\
{	\
	return kxf::IObject::QuerySelf(iid, static_cast<T&>(*this));	\
}

#define KxRTTI_QueryInterface_Extend(T, ...)	\
\
public:	\
void* DoQueryInterface(const kxf::IID& iid) noexcept override	\
{	\
	return kxf::IObject::QuerySelf<__VA_ARGS__>(iid, static_cast<T&>(*this));	\
}

namespace kxf
{
	class KX_API IObject
	{
		template<class T>
		friend constexpr IID RTTI::GetInterfaceID() noexcept;

		template<class T>
		friend class object_ptr;

		private:
			static constexpr IID ms_IID = NativeUUID{0, 0, 0, {0xC0, 0, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74}};

		protected:
			template<class T>
			static void* Cast(T& object, const IID& iid) noexcept
			{
				static_assert((std::is_base_of_v<IObject, T>), "T must inherit from 'IObject'");

				if (iid.IsOfType<T>())
				{
					return &object;
				}
				return nullptr;
			}

			template<class... Args, class TSelf>
			static void* QuerySelf(const IID& iid, TSelf& self) noexcept
			{
				if (iid.IsOfType<TSelf>())
				{
					return &self;
				}
				else if (void* ptr = nullptr; ((ptr = static_cast<Args&>(self).Args::DoQueryInterface(iid), ptr != nullptr) || ...))
				{
					return ptr;
				}
				return self.IObject::DoQueryInterface(iid);
			}

			template<class... Args>
			static void* UseAnyOf(const IID& iid, std::add_lvalue_reference_t<Args>&&... arg) noexcept
			{
				void* ptr = nullptr;
				if (((ptr = Cast<Args>(arg, iid), ptr != nullptr) || ...))
				{
					return ptr;
				}
				return nullptr;
			}
			
		protected:
			virtual void* DoQueryInterface(const IID& iid) noexcept
			{
				if (iid.IsOfType<IObject>())
				{
					return this;
				}
				return nullptr;
			}

			virtual size_t DoAddRef() noexcept
			{
				return 1;
			}
			virtual size_t DoReleaseRef() noexcept
			{
				return 1;
			}

		public:
			virtual ~IObject() = default;

		public:
			void* QueryInterface(const IID& iid) noexcept
			{
				return DoQueryInterface(iid);
			}
			const void* QueryInterface(const IID& iid) const noexcept
			{
				return const_cast<IObject*>(this)->DoQueryInterface(iid);
			}

			template<class T>
			object_ptr<T> QueryInterface() noexcept
			{
				return static_cast<T*>(this->QueryInterface(RTTI::GetInterfaceID<T>()));
			}
			
			template<class T>
			object_ptr<const T> QueryInterface() const noexcept
			{
				return static_cast<const T*>(this->QueryInterface(RTTI::GetInterfaceID<T>()));
			}

			template<class T>
			bool QueryInterface(object_ptr<T>& ptr) noexcept
			{
				ptr = this->QueryInterface<T>();
				return ptr != nullptr;
			}
			
			template<class T>
			bool QueryInterface(object_ptr<const T>& ptr) const noexcept
			{
				ptr = this->QueryInterface<T>();
				return ptr != nullptr;
			}
	};
}

namespace kxf::RTTI
{
	template<class T>
	class Interface: public virtual IObject
	{
		protected:
			void* DoQueryInterface(const IID& iid) noexcept override
			{
				return IObject::QuerySelf(iid, static_cast<T&>(*this));
			}
	};

	template<class TDerived, class... TBase>
	class ExtendInterface: public TBase...
	{
		protected:
			using TBaseInterface = typename ExtendInterface<TDerived, TBase...>;

		protected:
			void* DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");

				return IObject::QuerySelf<TBase...>(iid, static_cast<TDerived&>(*this));
			}

		public:
			ExtendInterface() = default;
	};

	template<class TDerived, class... TBase>
	class ImplementInterface: public TBase...
	{
		protected:
			using TBaseClass = typename ImplementInterface<TDerived, TBase...>;

		protected:
			void* DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");

				if (void* ptr = nullptr; ((ptr = TBase::DoQueryInterface(iid), ptr != nullptr) || ...))
				{
					return ptr;
				}
				return nullptr;
			}
	};
}
