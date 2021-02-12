#pragma once
#include "Common.h"
#include <utility>
#include <memory>

namespace kxf::Utility
{
	struct identity
	{
		static_assert(__cplusplus < 201703L, "use 'std::identity' instead");

		template<class T>
		constexpr auto operator()(T&& v) const noexcept -> decltype(std::forward<T>(v))
		{
			return std::forward<T>(v);
		}
	};

	template<size_t index>
	struct identity_tuple
	{
		template<class T>
		constexpr decltype(auto) operator()(T&& tuple) const noexcept
		{
			return std::get<index>(std::forward<T>(tuple));
		}
	};
}

namespace kxf::Utility
{
	struct AddressOf
	{
		public:
			template<class T>
			constexpr T* operator()(T& v) const noexcept
			{
				return &v;
			}

			template<class T>
			constexpr const T* operator()(const T& v) const noexcept
			{
				return &v;
			}

		public:
			template<class T, std::enable_if_t<!std::is_rvalue_reference_v<T> && std::is_class_v<T>, int> = 0>
			constexpr decltype(auto) operator()(T&& ptr) const noexcept
			{
				return ptr.get();
			}

		public:
			template<class T>
			T* operator()(std::unique_ptr<T>& ptr) const noexcept
			{
				return ptr.get();
			}

			template<class T>
			const T* operator()(const std::unique_ptr<T>& ptr) const noexcept
			{
				return ptr.get();
			}

			template<class T>
			T* operator()(std::shared_ptr<T>& ptr) const noexcept
			{
				return ptr.get();
			}

			template<class T>
			const T* operator()(const std::shared_ptr<T>& ptr) const noexcept
			{
				return ptr.get();
			}
	};

	struct ReferenceOf
	{
		public:
			template<class T>
			constexpr T& operator()(T* v) const noexcept
			{
				return *v;
			}

			template<class T>
			constexpr const T& operator()(const T* v) const noexcept
			{
				return *v;
			}

		public:
			template<class T, std::enable_if_t<!std::is_rvalue_reference_v<T>, int> = 0>
			constexpr decltype(auto) operator()(T&& ptr) const noexcept
			{
				return *ptr;
			}

		public:
			template<class T>
			T& operator()(std::unique_ptr<T>& ptr) const noexcept
			{
				return *ptr;
			}

			template<class T>
			const T& operator()(const std::unique_ptr<T>& ptr) const noexcept
			{
				return *ptr;
			}

			template<class T>
			T& operator()(std::shared_ptr<T>& ptr) const noexcept
			{
				return *ptr;
			}

			template<class T>
			const T& operator()(const std::shared_ptr<T>& ptr) const noexcept
			{
				return *ptr;
			}
	};
}
