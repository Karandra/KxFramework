#pragma once
#include "Common.h"
#include "IID.h"
#include "ObjectPtr.h"
#include "kxf/General/FlagSet.h"
#include <kxf/Utility/Common.h>
#include <kxf/Utility/TypeTraits.h>

namespace kxf::RTTI
{
	class ClassInfo;

	class QueryInfo final
	{
		private:
			void* m_Object = nullptr;
			std::unique_ptr<RTTI::ObjectDeleter> m_Deleter;

		private:
			template<class T = void>
			T* ExchangeObject(T* newObject = nullptr) noexcept
			{
				void* oldObject = m_Object;
				m_Object = const_cast<std::remove_const_t<T>*>(newObject);

				return static_cast<T*>(oldObject);
			}

		public:
			QueryInfo() noexcept = default;
			QueryInfo(void* ptr, std::unique_ptr<RTTI::ObjectDeleter> deleter = {}) noexcept
				:m_Object(ptr), m_Deleter(std::move(deleter))
			{
			}

			template<class T>
			QueryInfo(std::unique_ptr<T> ptr) noexcept
				:m_Object(ptr.release())
			{
				m_Deleter.reset(&RTTI::GetDefaultDeleter());
			}

			template<class T>
			QueryInfo(object_ptr<T> ptr) noexcept
				:m_Deleter(ptr.get_deleter())
			{
				m_Object = ptr.release();
			}

			QueryInfo(const QueryInfo&) = delete;
			QueryInfo(QueryInfo&& other) noexcept
				:m_Object(other.ExchangeObject()), m_Deleter(std::move(other.m_Deleter))
			{
			}
			~QueryInfo()
			{
				DestroyObject();
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

			void DestroyObject()
			{
				if (m_Deleter)
				{
					m_Deleter->Invoke(static_cast<IObject*>(m_Object));
					m_Deleter = nullptr;
				}
				m_Object = nullptr;
			}

			template<class T>
			object_ptr<T> TakeObject() noexcept
			{
				return object_ptr<T>(ExchangeObject<T>(), std::move(m_Deleter));
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

			QueryInfo& operator=(const QueryInfo&) = delete;
			QueryInfo& operator=(QueryInfo&& other) noexcept
			{
				DestroyObject();
				m_Object = other.ExchangeObject();
				m_Deleter = std::move(other.m_Deleter);

				return *this;
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
		friend const RTTI::ClassInfo& RTTI::GetClassInfo() noexcept;

		template<class T>
		friend class object_ptr;

		private:
			static constexpr IID ms_IID = NativeUUID{0, 0, 0, {0xC0, 0, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74}};
			static const RTTI::ClassInfo& ms_ClassInfo;

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
			virtual RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept = 0;

		public:
			IObject() noexcept = default;
			virtual ~IObject() = default;

		public:
			RTTI::QueryInfo QueryInterface(const IID& iid) noexcept
			{
				return this->DoQueryInterface(iid);
			}
			RTTI::QueryInfo QueryInterface(const IID& iid) const noexcept
			{
				return const_cast<IObject*>(this)->DoQueryInterface(iid);
			}

			template<class T>
			object_ptr<T> QueryInterface() noexcept
			{
				return this->DoQueryInterface(RTTI::GetInterfaceID<T>()).TakeObject<T>();
			}

			template<class T>
			object_ptr<const T> QueryInterface() const noexcept
			{
				return const_cast<IObject*>(this)->DoQueryInterface(RTTI::GetInterfaceID<T>()).TakeObject<const T>();
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
	template<class T, class... Args>
	std::unique_ptr<IObject> new_object(Args&&... arg)
	{
		static_assert(std::is_base_of_v<IObject, T>, "RTTI object required");

		auto instance = std::make_unique<T>(std::forward<Args>(arg)...);
		auto object = instance->IObject::QueryInterface<IObject>();
		wxASSERT_MSG(object.is_reference(), "IObject must not be dynamic");

		instance.release();
		return std::unique_ptr<IObject>(object.release());
	}
}

#define KxRTTI_DeclareIID(T, ...)	\
friend class kxf::IObject;	\
\
template<class T>	\
friend constexpr kxf::IID kxf::RTTI::GetInterfaceID() noexcept;	\
\
private:	\
	static constexpr kxf::IID ms_IID = kxf::NativeUUID __VA_ARGS__;

#define KxRTTI_DeclareIID_Using(T, iid)	\
friend class kxf::IObject;	\
\
template<class T>	\
friend constexpr kxf::IID kxf::RTTI::GetInterfaceID() noexcept;	\
\
private:	\
	static constexpr kxf::IID ms_IID = (iid);

#define KxRTTI_DeclareIID_External(T, ...)	\
namespace RTTI	\
{	\
	template<>	\
	constexpr kxf::IID GetInterfaceID<T>() noexcept	\
	{	\
		return kxf::NativeUUID __VA_ARGS__;	\
	}	\
}
