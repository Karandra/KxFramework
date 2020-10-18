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
kxf::RTTI::QueryInfo DoQueryInterface(const kxf::IID& iid) noexcept override	\
{	\
	return kxf::IObject::QuerySelf(iid, static_cast<T&>(*this));	\
}

#define KxRTTI_QueryInterface_Extend(T, ...)	\
\
public:	\
kxf::RTTI::QueryInfo DoQueryInterface(const kxf::IID& iid) noexcept override	\
{	\
	return kxf::IObject::QuerySelf<__VA_ARGS__>(iid, static_cast<T&>(*this));	\
}

namespace kxf::RTTI
{
	class QueryInfo final
	{
		private:
			void* m_Object = nullptr;
			std::unique_ptr<RTTI::ObjectDeleter> m_Deleter;

		public:
			QueryInfo() noexcept = default;
			QueryInfo(void* ptr, std::unique_ptr<RTTI::ObjectDeleter> deleter = {}) noexcept
				:m_Object(ptr), m_Deleter(std::move(deleter))
			{
			}

		public:
			bool is_null() const noexcept
			{
				return m_Object == nullptr;
			}
			bool is_reference() const noexcept
			{
				return m_Deleter == nullptr;
			}
			
			const void* get() const noexcept
			{
				return m_Object;
			}
			void* get() noexcept
			{
				return m_Object;
			}

			template<class T>
			object_ptr<T> TakeObject() noexcept
			{
				return object_ptr<T>(static_cast<T*>(m_Object), std::move(m_Deleter));
			}

			template<class T>
			object_ptr<T> ReferenceObject() const noexcept
			{
				return object_ptr<T>(static_cast<T*>(m_Object));
			}

		public:
			explicit operator bool() const noexcept
			{
				return !is_null();
			}
			bool operator!() const noexcept
			{
				return is_null();
			}
	};
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
			static RTTI::QueryInfo Cast(T& object, const IID& iid) noexcept
			{
				static_assert((std::is_base_of_v<IObject, T>), "T must inherit from 'IObject'");

				if (iid.IsOfType<T>())
				{
					return &object;
				}
				return nullptr;
			}

			template<class... Args, class TSelf>
			static RTTI::QueryInfo QuerySelf(const IID& iid, TSelf& self) noexcept
			{
				if (iid.IsOfType<TSelf>())
				{
					return &self;
				}
				else if (RTTI::QueryInfo ptr; ((ptr = static_cast<Args&>(self).Args::DoQueryInterface(iid), !ptr.is_null()) || ...))
				{
					return ptr;
				}
				return self.IObject::DoQueryInterface(iid);
			}

			template<class... Args>
			static RTTI::QueryInfo UseAnyOf(const IID& iid, std::add_lvalue_reference_t<Args>&&... arg) noexcept
			{
				void* ptr = nullptr;
				if (((ptr = Cast<Args>(arg, iid), ptr != nullptr) || ...))
				{
					return ptr;
				}
				return nullptr;
			}
			
		protected:
			virtual RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept
			{
				if (iid.IsOfType<IObject>())
				{
					return this;
				}
				return nullptr;
			}

		public:
			virtual ~IObject() = default;

		public:
			RTTI::QueryInfo QueryInterface(const IID& iid) noexcept
			{
				return DoQueryInterface(iid);
			}
			const RTTI::QueryInfo QueryInterface(const IID& iid) const noexcept
			{
				return const_cast<IObject*>(this)->DoQueryInterface(iid);
			}

			template<class T>
			object_ptr<T> QueryInterface() noexcept
			{
				return this->QueryInterface(RTTI::GetInterfaceID<T>()).TakeObject<T>();
			}
			
			template<class T>
			object_ptr<const T> QueryInterface() const noexcept
			{
				return const_cast<IObject*>(this)->QueryInterface(RTTI::GetInterfaceID<T>()).TakeObject<const T>();
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
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
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
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
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
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");

				if (RTTI::QueryInfo ptr; ((ptr = TBase::DoQueryInterface(iid), !ptr.is_null()) || ...))
				{
					return ptr;
				}
				return nullptr;
			}
	};
}
