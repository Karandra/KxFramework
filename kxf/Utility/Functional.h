#pragma once
#include "Common.h"
#include <utility>
#include <memory>

namespace kxf::Utility
{
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
			template<class T> requires(!std::is_rvalue_reference_v<T> && std::is_class_v<T>)
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
			template<class T> requires(!std::is_rvalue_reference_v<T>)
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

	struct UnfancyPtr
	{
		public:
			template<class T> requires(std::is_class_v<T>)
			constexpr auto operator()(T& v) const noexcept
			{
				return v.get();
			}

			template<class T> requires(std::is_class_v<T>)
			constexpr auto operator()(const T& v) const noexcept
			{
				return v.get();
			}
	};
}
