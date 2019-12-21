#pragma once
#include "Common.h"
#include <Kx/Utility/TypeTraits.h>

class KX_API KxIID final
{
	public:
		struct GUID
		{
			uint32_t Data1 = 0;
			uint16_t Data2 = 0;
			uint16_t Data3 = 0;
			uint8_t Data4[8] = {};

			constexpr bool operator==(const GUID& other) const noexcept
			{
				if (Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3)
				{
					for (size_t i = 0; i < sizeof(GUID::Data4); i++)
					{
						if (Data4[i] != other.Data4[i])
						{
							return false;
						}
					}
					return true;
				}
				return false;
			}
			constexpr bool operator!=(const GUID& other) const noexcept
			{
				return !(*this == other);
			}
		};

	public:
		template<class T>
		constexpr static KxIID FromType() noexcept
		{
			return T::IID;
		}

	private:
		GUID m_GUID = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

	public:
		constexpr KxIID() noexcept = default;
		constexpr KxIID(const GUID& guid) noexcept
			:m_GUID(guid)
		{
		}
		
	public:
		constexpr bool IsNull() const noexcept
		{
			return m_GUID == KxIID().m_GUID;
		}

		template<class T>
		constexpr bool IsOfType() const noexcept
		{
			return FromType<T>().m_GUID == m_GUID;
		}

		constexpr bool operator==(const KxIID& other) const noexcept
		{
			return this == &other || m_GUID == other.m_GUID;
		}
		constexpr bool operator!=(const KxIID& other) const noexcept
		{
			return !(*this == other);
		}
		
		constexpr KxIID& operator=(const GUID& guid) noexcept
		{
			m_GUID = guid;
			return *this;
		}
};

#define KxDecalreIID(T, ...)	\
\
friend class KxIID;	\
friend constexpr KxIID KxIID::FromType<T>() noexcept;	\
\
private:	\
	static constexpr KxIID IID = KxIID::GUID __VA_ARGS__;	\
\
public:	\
	KxIID GetIID() const noexcept override	\
	{	\
		return T::IID;	\
	}	\


class KX_API KxIObject
{
	friend class KxIID;
	friend constexpr KxIID KxIID::FromType<KxIObject>() noexcept;

	private:
		static constexpr KxIID IID = KxIID::GUID {0, 0, 0, {0xC0, 0, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74}};

	protected:
		template<class T>
		static void* Cast(T& object, const KxIID& iid) noexcept
		{
			static_assert((std::is_base_of_v<KxIObject, T>), "T must inherit from 'KxIObject'");

			if (iid.IsOfType<T>())
			{
				return &object;
			}
			return nullptr;
		}

		template<class... Args, class TSelf>
		static void* QuerySelf(const KxIID& iid, TSelf& self) noexcept
		{
			if (iid.IsOfType<TSelf>())
			{
				return &self;
			}
			else if (void* ptr = nullptr; ((ptr = static_cast<Args&>(self).Args::QueryInterface(iid), ptr != nullptr) || ...))
			{
				return ptr;
			}
			return self.KxIObject::QueryInterface(iid);
		}

		template<class... Args>
		static void* UseAnyOf(const KxIID& iid, std::add_lvalue_reference_t<Args>&&... arg) noexcept
		{
			void* ptr = nullptr;
			return ((ptr = KxIObject::Cast<Args>(arg, iid), ptr != nullptr) || ...);
		}

	public:
		virtual ~KxIObject() = default;

	public:
		virtual KxIID GetIID() const noexcept = 0
		{
			return IID;
		}

		virtual void* QueryInterface(const KxIID& iid) noexcept
		{
			if (iid.IsOfType<KxIObject>())
			{
				return this;
			}
			return nullptr;
		}
		const void* QueryInterface(const KxIID& iid) const noexcept
		{
			return const_cast<KxIObject*>(this)->QueryInterface(iid);
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
			KxIID GetIID() const noexcept override = 0;

			using KxIObject::QueryInterface;
			void* QueryInterface(const KxIID& iid) noexcept override
			{
				return KxIObject::QuerySelf(iid, static_cast<T&>(*this));
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
			KxIID GetIID() const noexcept override = 0;

			using KxIObject::QueryInterface;
			void* QueryInterface(const KxIID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<KxIObject, TBase> && ...), "[...] must inherit from 'KxIObject'");

				return KxIObject::QuerySelf<TBase...>(iid, static_cast<TDerived&>(*this));
			}
	};

	template<class TDerived, class... TBase>
	class ImplementInterface: public TBase...
	{
		public:
			KxIID GetIID() const noexcept override
			{
				if constexpr(KxUtility::CountOfParameterPack<TBase...>() == 1)
				{
					return KxUtility::NthTypeOf<0, TBase...>::GetIID();
				}
				return {};
			}

			using KxIObject::QueryInterface;
			void* QueryInterface(const KxIID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<KxIObject, TBase> && ...), "[...] must inherit from 'KxIObject'");

				if (void* ptr = nullptr; ((ptr = TBase::QueryInterface(iid), ptr != nullptr) || ...))
				{
					return ptr;
				}
				return nullptr;
			}
	};
}
