/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include <memory>

namespace KxFunctional
{
	template<class T> struct is_unique_ptr: std::false_type
	{
	};

	template<class T, class D>
	struct is_unique_ptr<std::unique_ptr<T, D>>: std::true_type
	{
	};

	template<class T, class D>
	struct is_unique_ptr<const std::unique_ptr<T, D>>: std::true_type
	{
	};

	template<class T, class D>
	struct is_unique_ptr<std::unique_ptr<T, D>&>: std::true_type
	{
	};

	template<class T, class D>
	struct is_unique_ptr<const std::unique_ptr<T, D>&>: std::true_type
	{
	};

	template<class T> inline constexpr bool is_unique_ptr_v = is_unique_ptr<T>::value;
}
