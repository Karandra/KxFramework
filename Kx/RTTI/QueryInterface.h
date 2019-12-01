#pragma once
#include "Common.h"
#include <Kx/Utility/TypeTraits.h>
#include <typeinfo>
class KxIObject;

class KX_API KxIID final
{
	public:
		template<class T> static KxIID FromType() noexcept
		{
			return typeid(T);
		}

	private:
		const std::type_info& m_TypeInfo;

	public:
		KxIID(const std::type_info& typeInfo) noexcept
			:m_TypeInfo(typeInfo)
		{
		}
		KxIID(const KxIID&) = delete;
		
	public:
		template<class T> bool IsOfType() const noexcept
		{
			return m_TypeInfo == typeid(T);
		}

		bool operator==(const KxIID& other) noexcept
		{
			return m_TypeInfo == other.m_TypeInfo;
		}
		bool operator!=(const KxIID& other) noexcept
		{
			return m_TypeInfo != other.m_TypeInfo;
		}
		KxIID& operator=(const KxIID&) = delete;
};

class KX_API KxIObject
{
	protected:
		template<class T>
		static bool Cast(T& object, const KxIID& iid, void*& ptr) noexcept
		{
			static_assert((std::is_base_of_v<KxIObject, T>), "T must inherit from 'KxIObject'");

			if (iid.IsOfType<T>())
			{
				ptr = &object;
				return true;
			}
			return false;
		}

		template<class... Args, class TSelf>
		static bool QuerySelf(const KxIID& iid, void*& ptr, TSelf& self) noexcept
		{
			if (iid.IsOfType<TSelf>())
			{
				ptr = &self;
				return true;
			}
			else if (void* result = nullptr; (static_cast<Args&>(self).Args::QueryInterface(iid, result) || ...))
			{
				ptr = result;
				return true;
			}
			return self.KxIObject::QueryInterface(iid, ptr);
		}

		template<class... Args>
		static bool UseAnyOf(const KxIID& iid, void*& ptr, std::add_lvalue_reference_t<Args>&&... arg) noexcept
		{
			return (KxIObject::Cast<Args>(arg, iid, ptr) || ...);
		}

	public:
		virtual ~KxIObject() = default;

	public:
		virtual bool QueryInterface(const KxIID& iid, void*& ptr) noexcept
		{
			if (iid.IsOfType<KxIObject>())
			{
				ptr = this;
				return true;
			}
			return false;
		}
		
		void* QueryInterface(const KxIID& iid) noexcept
		{
			void* ptr = nullptr;
			this->QueryInterface(iid, ptr);
			return ptr;
		}
		const void* QueryInterface(const KxIID& iid) const noexcept
		{
			void* ptr = nullptr;
			const_cast<KxIObject*>(this)->QueryInterface(iid, ptr);
			return ptr;
		}

		template<class T> T* QueryInterface() noexcept
		{
			return static_cast<T*>(this->QueryInterface(KxIID::FromType<T>()));
		}
		template<class T> const T* QueryInterface() const noexcept
		{
			return static_cast<const T*>(this->QueryInterface(KxIID::FromType<T>()));
		}

		template<class T> bool QueryInterface(T*& ptr) noexcept
		{
			ptr = this->QueryInterface<T>();
			return ptr != nullptr;
		}
		template<class T> bool QueryInterface(const T*& ptr) const noexcept
		{
			ptr = this->QueryInterface<T>();
			return ptr != nullptr;
		}
};

namespace KxRTTI
{
	template<class T>
	class Interface: public virtual KxIObject
	{
		public:
			using KxIObject::QueryInterface;
			bool QueryInterface(const KxIID& iid, void*& ptr) noexcept override
			{
				return KxIObject::QuerySelf(iid, ptr, static_cast<T&>(*this));
			}
	};

	template<class TDerived, class... TBase>
	class ExtendInterface: public TBase...
	{
		public:
			ExtendInterface() = default;
			template<class... Args> ExtendInterface(Args&&... arg) noexcept
				:KxUtility::NthTypeOf<0, TBase...>(std::forward<Args>(arg)...)
			{
			}

		public:
			using KxIObject::QueryInterface;
			bool QueryInterface(const KxIID& iid, void*& ptr) noexcept override
			{
				static_assert((std::is_base_of_v<KxIObject, TBase> && ...), "[...] must inherit from 'KxIObject'");

				return KxIObject::QuerySelf<TBase...>(iid, ptr, static_cast<TDerived&>(*this));
			}
	};
}