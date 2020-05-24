#pragma once
#include "../Common.h"

namespace kxf::Private::ExtraData
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

		union
		{
			std::remove_reference_t<T> m_Typed;
			void* m_Untyped = nullptr;
		} value;
		value.m_Untyped = data;

		return value.m_Typed;
	}

	template<class T>
	void* SetUntypedData(T&& data)
	{
		AssertUntypedStorageType<T>();

		union
		{
			std::remove_reference_t<T> m_Typed;
			void* m_Untyped = nullptr;
		} value;
		value.m_Typed = std::forward<T>(data);

		return value.m_Untyped;
	}
}
