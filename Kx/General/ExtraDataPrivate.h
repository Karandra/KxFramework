#pragma once
#include "Common.h"

namespace KxFramework::Private::ExtraData
{
	template<class T>
	constexpr void AssertUntypedStorageType()
	{
		using Tx = std::remove_reference_t<T>;
		static_assert(sizeof(Tx) <= sizeof(void*) && (std::is_trivially_copyable_v<Tx>), "invalid type for untyped storage");
	}

	template<class T = void*>
	T GetUntypedData(void* data)
	{
		AssertUntypedStorageType<T>();

		return reinterpret_cast<T>(data);
	}

	template<class T>
	void* SetUntypedData(T&& data)
	{
		AssertUntypedStorageType<T>();

		return reinterpret_cast<void*>(data);
	}
}
