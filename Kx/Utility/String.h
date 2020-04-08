#pragma once
#include "Kx/Common.hpp"
#include "Kx/General/String.h"
#include <unordered_map>
#include <unordered_set>

namespace KxFramework::Utility
{
	struct StringEqualToNoCase
	{
		template<class T>
		bool operator()(T&& left, T&& right) const noexcept
		{
			return String::Compare(std::forward<T>(left), std::forward<T>(right), StringOpFlag::IgnoreCase) == 0;
		}
	};

	struct StringHashNoCase
	{
		// From Boost
		template<class T>
		static void hash_combine(size_t& seed, const T& v) noexcept
		{
			std::hash<T> hasher;
			seed ^= hasher(v) + size_t(0x9e3779b9u) + (seed << 6) + (seed >> 2);
		}

		template<class T>
		size_t operator()(T&& value) const noexcept
		{
			size_t hashValue = 0;
			for (const auto& c: value)
			{
				hash_combine(hashValue, String::ToLower(c).GetValue());
			}
			return hashValue;
		}
	};

	template<class TKey, class TValue>
	using UnorderedMapNoCase = std::unordered_map<TKey, TValue, StringHashNoCase, StringEqualToNoCase>;
	
	template<class TValue>
	using UnorderedSetNoCase = std::unordered_set<TValue, StringHashNoCase, StringEqualToNoCase>;
}
