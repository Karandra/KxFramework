#pragma once
#include "KxFramework/KxFramework.h"
#include <any>

class KX_API KxAny final
{
	private:
		std::any m_Any;

	private:
		template<class TFunc>
		bool ResetOnException(TFunc&& func) noexcept
		{
			try
			{
				std::invoke(func);
				return true;
			}
			catch (...)
			{
				MakeNull();
				return false;
			}
		}

		template<class T>
		static bool ConvertTo(const KxAny& any, T& value) noexcept
		{
			if constexpr(std::is_same_v<T, wxString>)
			{
				// Convert to a string
				if (value = any.IntToString(); !value.IsEmpty())
				{
					return true;
				}
				else if (value = any.FloatToString(); !value.IsEmpty())
				{
					return true;
				}
				else if (value = any.BoolToString(); !value.IsEmpty())
				{
					return true;
				}
			}
			else if constexpr(std::is_same_v<T, bool>)
			{
				// Convert to a bool value
				if (auto ptr = any.AsPtr<uint64_t>())
				{
					value = *ptr != 0;
					return true;
				}
				else if (auto ptr = any.AsPtr<double>())
				{
					value = *ptr != 0;
					return true;
				}
				else if (auto opt = any.StringToBool())
				{
					value = *opt;
					return true;
				}
			}
			else if constexpr(std::is_integral_v<T> || std::is_enum_v<T>)
			{
				using TIntType = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;

				// Convert to an integer
				if constexpr (std::is_signed_v<TIntType>)
				{
					// Convert to signed int
					if (const wxString* ptr = any.AsPtr<wxString>())
					{
						if (long long iValue = 0; ptr->ToLongLong(&iValue))
						{
							value = static_cast<T>(iValue);
							return true;
						}
					}
				}
				else if constexpr(std::is_unsigned_v<TIntType>)
				{
					// Convert to unsigned int
					if (const wxString* ptr = any.AsPtr<wxString>())
					{
						if (unsigned long long iValue = 0; ptr->ToULongLong(&iValue))
						{
							value = static_cast<T>(iValue);
							return true;
						}
					}
				}

				if (auto ptr = any.AsPtr<double>())
				{
					value = static_cast<T>(*ptr);
					return true;
				}
				else if (const bool* ptr = any.AsPtr<bool>())
				{
					value = static_cast<T>(*ptr ? 1 : 0);
					return true;
				}
			}
			else if constexpr(std::is_floating_point_v<T>)
			{
				// Convert to float or double
				if (const wxString* ptr = any.AsPtr<wxString>())
				{
					if (double dValue = 0; ptr->ToCDouble(&dValue))
					{
						value = dValue;
						return true;
					}
				}
				else if (auto ptr = any.AsPtr<int64_t>())
				{
					value = *ptr;
					return true;
				}
				else if (const bool* ptr = any.AsPtr<bool>())
				{
					value = *ptr ? 1 : 0;
					return true;
				}
			}
			return false;
		}

		template<class T> T* AsPtr() noexcept
		{
			return std::any_cast<T>(&m_Any);
		}
		template<class T> const T* AsPtr() const noexcept
		{
			return std::any_cast<T>(&m_Any);
		}

		wxString IntToString() const noexcept;
		wxString FloatToString() const noexcept;
		wxString BoolToString() const noexcept;
		std::optional<bool> StringToBool() const noexcept;

	public:
		KxAny() = default;
		
		explicit KxAny(const KxAny&) noexcept = default;
		explicit KxAny(KxAny&&) noexcept = default;
		explicit KxAny(const std::any& other) noexcept
		{
			Assign(other);
		}
		explicit KxAny(std::any&& other) noexcept
			:m_Any(std::move(other))
		{
		}

		template<class T> KxAny(T&& value) noexcept
		{
			Assign(std::forward<T>(value));
		}

	public:
		bool IsNull() const noexcept
		{
			return !m_Any.has_value();
		}
		bool HasValue() const noexcept
		{
			return m_Any.has_value();
		}
		void MakeNull() noexcept
		{
			m_Any.reset();
		}
		
		void Swap(KxAny& other) noexcept
		{
			m_Any.swap(other.m_Any);
		}
		void Swap(std::any& other) noexcept
		{
			m_Any.swap(other);
		}

		const std::type_info& GetTypeInfo() const noexcept
		{
			return m_Any.type();
		}
		bool HasSameType(const KxAny& other) const noexcept
		{
			return GetTypeInfo() == other.GetTypeInfo();
		}
		
		template<class T> bool CheckType() const noexcept
		{
			static_assert(!std::is_void_v<T>, "T must not be void");

			if constexpr(std::is_reference_v<T>)
			{
				return GetTypeInfo() == typeid(std::remove_reference_t<T>*);
			}
			else
			{
				return GetTypeInfo() == typeid(T);
			}
		}

		// Getters
		const std::any& GetAny() const noexcept
		{
			return m_Any;
		}
		std::any& GetAny() noexcept
		{
			return m_Any;
		}

		template<class T> T As() const
		{
			return std::any_cast<T>(m_Any);
		}
		template<class T> T As()
		{
			if constexpr(std::is_rvalue_reference_v<T>)
			{
				return std::any_cast<T>(std::move(*this));
			}
			else
			{
				return std::any_cast<T>(m_Any);
			}
		}
		
		template<class T, class TDefault> T AsOr(TDefault&& defaultValue) const
		{
			if (auto ptr = AsPtr<T>())
			{
				return *ptr;
			}
			return std::forward<TDefault>(defaultValue);
		}
		template<class T, class TDefault> T AsOr(TDefault&& defaultValue)
		{
			if (auto ptr = AsPtr<T>())
			{
				return std::forward<T>(*ptr);
			}
			return std::forward<TDefault>(defaultValue);
		}

		template<class T> bool As(const T*& value) const noexcept
		{
			value = AsPtr<T>();
			return value != nullptr;
		}
		template<class T> bool As(T*& value) noexcept
		{
			value = AsPtr<T>();
			return value != nullptr;
		}

		template<class T> bool GetAs(T& value) const noexcept
		{
			if (const T* ptr = AsPtr<T>())
			{
				value = *ptr;
				return true;
			}
			return ConvertTo(*this, value);
		}
		template<class T> bool GetAs(T&& value) noexcept
		{
			if (const T* ptr = AsPtr<T>())
			{
				value = std::forward<T>(*ptr);
				return true;
			}
			return ConvertTo(*this, value);
		}

		template<class T> T GetAs() const noexcept
		{
			static_assert(std::is_default_constructible_v<T>, "T must be default constructible");

			T value;
			GetAs(value);
			return value;
		}
		template<class T> T GetAs() noexcept
		{
			static_assert(std::is_default_constructible_v<T>, "T must be default constructible");

			T value;
			GetAs(value);
			return value;
		}

		// Assignment
		template<class T> KxAny& Assign(T&& value) noexcept
		{
			ResetOnException([&]()
			{
				if constexpr(std::is_enum_v<T>)
				{
					m_Any = static_cast<std::underlying_type_t<T>>(value);
				}
				else
				{
					m_Any = std::forward<T>(value);
				}
			});
			return *this;
		}
		
		KxAny& Assign(const KxAny& other) noexcept
		{
			ResetOnException([&]()
			{
				m_Any = other.m_Any;
			});
			return *this;
		}
		KxAny& Assign(KxAny&& other) noexcept
		{
			m_Any = std::move(other.m_Any);
			return *this;
		}
		KxAny& Assign(const std::any& other) noexcept
		{
			ResetOnException([&]()
			{
				m_Any = other;
			});
			return *this;
		}
		KxAny& Assign(std::any&& other) noexcept
		{
			m_Any = std::move(other);
			return *this;
		}

		KxAny& Assign(const char* value) noexcept
		{
			ResetOnException([&]()
			{
				m_Any = wxString(value);
			});
			return *this;
		}
		KxAny& Assign(const wchar_t* value) noexcept
		{
			ResetOnException([&]()
			{
				m_Any = wxString(value);
			});
			return *this;
		}

	public:
		explicit operator bool() const noexcept
		{
			return HasValue();
		}
		bool operator!() noexcept
		{
			return IsNull();
		}

		template<class T> KxAny& operator=(T&& value) noexcept
		{
			return Assign(std::forward<T>(value));
		}
};

namespace std
{
	inline void swap(KxAny& left, KxAny& right) noexcept
	{
		left.Swap(right);
	}
	inline void swap(KxAny& left, std::any& right) noexcept
	{
		left.Swap(right);
	}
	inline void swap(std::any& left, KxAny& right) noexcept
	{
		right.Swap(left);
	}
}
