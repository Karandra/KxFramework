#pragma once
#include "Common.h"
#include <Kx/General/NativeUUID.h>
#include <Kx/Utility/Common.h>
#include <Kx/Utility/TypeTraits.h>

namespace KxFramework
{
	class KX_API IID final
	{
		public:
			template<class T>
			constexpr static IID FromType() noexcept
			{
				return T::ms_IID;
			}

		private:
			NativeUUID m_ID;

		public:
			constexpr IID() noexcept = default;
			constexpr IID(const NativeUUID& guid) noexcept
				:m_ID(guid)
			{
			}
			
		public:
			constexpr bool IsNull() const noexcept
			{
				return m_ID.IsNull();
			}
			constexpr NativeUUID ToNativeUUID() const noexcept
			{
				return m_ID;
			}

			template<class T>
			constexpr bool IsOfType() const noexcept
			{
				return FromType<T>().m_ID == m_ID;
			}

			constexpr bool operator==(const IID& other) const noexcept
			{
				return this == &other || m_ID == other.m_ID;
			}
			constexpr bool operator!=(const IID& other) const noexcept
			{
				return !(*this == other);
			}
			
			constexpr IID& operator=(const NativeUUID& uuid) noexcept
			{
				m_ID = uuid;
				return *this;
			}
	};
}

namespace std
{
	template<>
	struct hash<KxFramework::IID>
	{
		size_t operator()(const KxFramework::IID& iid) const noexcept;
	};
}

#define KxDecalreIID(T, ...)	\
\
friend class KxFramework::IID;	\
friend constexpr KxFramework::IID KxFramework::IID::FromType<T>() noexcept;	\
\
private:	\
	static constexpr KxFramework::IID ms_IID = KxFramework::NativeUUID __VA_ARGS__;	\
\
public:	\
	KxFramework::IID GetIID() const noexcept override	\
	{	\
		return T::ms_IID;	\
	}	\


namespace KxFramework
{
	class KX_API IObject
	{
		friend class IID;
		friend constexpr IID IID::FromType<IObject>() noexcept;

		private:
			static constexpr IID ms_IID = NativeUUID{0, 0, 0, {0xC0, 0, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74}};

		protected:
			template<class T>
			static void* Cast(T& object, const IID& iid) noexcept
			{
				static_assert((std::is_base_of_v<IObject, T>), "T must inherit from 'KxIObject'");

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
				else if (void* ptr = nullptr; ((ptr = static_cast<Args&>(self).Args::QueryInterface(iid), ptr != nullptr) || ...))
				{
					return ptr;
				}
				return self.IObject::QueryInterface(iid);
			}

			template<class... Args>
			static void* UseAnyOf(const IID& iid, std::add_lvalue_reference_t<Args>&&... arg) noexcept
			{
				void* ptr = nullptr;
				return ((ptr = IObject::Cast<Args>(arg, iid), ptr != nullptr) || ...);
			}

		public:
			virtual ~IObject() = default;

		public:
			virtual IID GetIID() const noexcept = 0
			{
				return ms_IID;
			}

			virtual void* QueryInterface(const IID& iid) noexcept
			{
				if (iid.IsOfType<IObject>())
				{
					return this;
				}
				return nullptr;
			}
			const void* QueryInterface(const IID& iid) const noexcept
			{
				return const_cast<IObject*>(this)->QueryInterface(iid);
			}

			template<class T> T* QueryInterface() noexcept
			{
				return static_cast<T*>(this->QueryInterface(IID::FromType<T>()));
			}
			template<class T> const T* QueryInterface() const noexcept
			{
				return static_cast<const T*>(this->QueryInterface(IID::FromType<T>()));
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
}

namespace KxFramework::RTTI
{
	template<class T>
	class Interface: public virtual IObject
	{
		public:
			IID GetIID() const noexcept override = 0;

			using IObject::QueryInterface;
			void* QueryInterface(const IID& iid) noexcept override
			{
				return IObject::QuerySelf(iid, static_cast<T&>(*this));
			}
	};

	template<class TDerived, class... TBase>
	class ExtendInterface: public TBase...
	{
		public:
			ExtendInterface() = default;
			template<class... Args> ExtendInterface(Args&&... arg) noexcept
				:Utility::NthTypeOf<0, TBase...>(std::forward<Args>(arg)...)
			{
			}

		public:
			IID GetIID() const noexcept override = 0;

			using IObject::QueryInterface;
			void* QueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'KxIObject'");

				return IObject::QuerySelf<TBase...>(iid, static_cast<TDerived&>(*this));
			}
	};

	template<class TDerived, class... TBase>
	class ImplementInterface: public TBase...
	{
		public:
			IID GetIID() const noexcept override
			{
				if constexpr(Utility::CountOfParameterPack<TBase...>() == 1)
				{
					return Utility::NthTypeOf<0, TBase...>::GetIID();
				}
				return {};
			}

			using IObject::QueryInterface;
			void* QueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'KxIObject'");

				if (void* ptr = nullptr; ((ptr = TBase::QueryInterface(iid), ptr != nullptr) || ...))
				{
					return ptr;
				}
				return nullptr;
			}
	};
}
