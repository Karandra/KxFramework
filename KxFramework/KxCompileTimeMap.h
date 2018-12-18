#pragma once
#include "KxFramework/KxFramework.h"
#include <optional>

namespace KxCompileTimeMap
{
	template<class TKey, class TValue, TKey ta_Key, TValue ta_Value>
	struct Item
	{
		constexpr static TKey GetKey()
		{
			return ta_Key;
		}
		constexpr static TValue GetValue()
		{
			return ta_Value;
		}
	};

	template<class TKey, class TValue, TValue ta_Default, class...> class Map;
	template<class TKey, class TValue, TValue ta_Default> class Map<TKey, TValue, ta_Default>
	{
		public:
			using TValueOpt = std::optional<TValue>;

		public:
			template<TKey t_Key> constexpr static bool HasKey()
			{
				return false;
			};
			
			template<TKey> constexpr static TValue GetValue()
			{
				return ta_Default;
			};
			template<TKey> constexpr static TValueOpt TryGetValue()
			{
				return std::nullopt;
			};
	};

	template<class TKey, class TValue, TValue ta_Default, TKey ta_Key, TValue ta_Value, class... TRest>
	class Map<TKey, TValue, ta_Default, Item<TKey, TValue, ta_Key, ta_Value>, TRest...>
	{
		private:
			using TSelfRef = Map<TKey, TValue, ta_Default, TRest...>;

		public:
			using TValueOpt = std::optional<TValue>;

		public:
			template<TKey ta_GetKey> constexpr static bool HasKey()
			{
				return (ta_GetKey == ta_Key) ? true : TSelfRef::template HasKey<ta_GetKey>();
			};
			
			template<TKey ta_GetKey> constexpr static TValue GetValue()
			{
				return (ta_GetKey == ta_Key) ? ta_Value : TSelfRef::template GetValue<ta_GetKey>();
			};
			template<TKey ta_GetKey> constexpr static TValueOpt TryGetValue()
			{
				return (ta_GetKey == ta_Key) ? TValueOpt(ta_Value) : TSelfRef::template TryGetValue<ta_GetKey>();
			};
	};
}
