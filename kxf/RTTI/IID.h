#pragma once
#include "Common.h"
#include <kxf/General/NativeUUID.h>

namespace kxf::RTTI
{
	template<class T>
	constexpr IID GetInterfaceID() noexcept
	{
		return T::ms_IID;
	}
}

namespace kxf
{
	class KX_API IID final
	{
		private:
			NativeUUID m_ID;

		public:
			constexpr IID() noexcept = default;
			constexpr IID(const NativeUUID& uuid) noexcept
				:m_ID(uuid)
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
				return *this == RTTI::GetInterfaceID<T>();
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
	struct hash<kxf::IID>
	{
		constexpr size_t operator()(const kxf::IID& iid) const noexcept
		{
			return iid.ToNativeUUID().GetHash();
		}
	};
}
