#pragma once
#include "Common.h"
#include <kxf/General/NativeUUID.h>

namespace kxf
{
	class UniversallyUniqueID;
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
			UniversallyUniqueID ToUniversallyUniqueID() const noexcept;

			template<class T>
			constexpr bool IsOfType() const noexcept
			{
				return *this == RTTI::GetInterfaceID<T>();
			}

		public:
			explicit constexpr operator bool() const noexcept
			{
				return !m_ID.IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return m_ID.IsNull();
			}

			constexpr bool operator==(const IID& other) const noexcept
			{
				return m_ID == other.m_ID;
			}
			constexpr bool operator!=(const IID& other) const noexcept
			{
				return m_ID != other.m_ID;
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
