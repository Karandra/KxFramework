#pragma once
#include "Common.h"
#include "ErrorCodeValue.h"
#include "Kx/General/String.h"

namespace KxFramework
{
	class KX_API ErrorCode final
	{
		private:
			int64_t m_Value = -1;
			ErrorCodeCategory m_Category = ErrorCodeCategory::Unknown;

		private:
			template<class T>
			constexpr ErrorCode& AssignCode(T errorCode) noexcept
			{
				m_Value = static_cast<int64_t>(errorCode.GetValue());
				m_Category = T::GetCategory();

				return *this;
			}

			template<class T>
			constexpr bool IsEqualValue(T errorCode) const noexcept
			{
				return m_Category == T::GetCategory() && static_cast<typename T::TValueType>(m_Value) == errorCode.GetValue();
			}

			template<class T>
			constexpr std::optional<T> GetAsCode() const noexcept
			{
				if (m_Category == T::GetCategory())
				{
					return T(static_cast<typename T::TValueType>(m_Value));
				}
				return {};
			}

		public:
			constexpr ErrorCode() noexcept = default;
			constexpr ErrorCode(GenericError errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			constexpr ErrorCode(Win32Error errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			constexpr ErrorCode(NtStatus errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			constexpr ErrorCode(HResult errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			constexpr ErrorCode(ErrorCode&& other) noexcept
			{
				*this = std::move(other);
			}
			constexpr ErrorCode(const ErrorCode&) noexcept = default;

		public:
			constexpr ErrorCodeCategory GetCategory() const noexcept
			{
				return m_Category;
			}
			constexpr std::optional<GenericError> GetGeneric() const noexcept
			{
				return GetAsCode<GenericError>();
			}
			constexpr std::optional<Win32Error> GetWin32() const noexcept
			{
				return GetAsCode<Win32Error>();
			}
			constexpr std::optional<NtStatus> GetNtStatus() const noexcept
			{
				return GetAsCode<NtStatus>();
			}
			constexpr std::optional<HResult> GetHResult() const noexcept
			{
				return GetAsCode<HResult>();
			}

			std::optional<Win32Error> ConvertToWin32() const noexcept;
			std::optional<NtStatus> ConvertToNtStatus() const noexcept;
			std::optional<HResult> ConvertToHResult() const noexcept;

			constexpr bool IsKnown() const noexcept
			{
				return m_Category != ErrorCodeCategory::Unknown;
			}
			constexpr bool IsSuccess() const noexcept
			{
				switch (m_Category)
				{
					case ErrorCodeCategory::Generic:
					{
						return GenericError(m_Value).IsSuccess();
					}
					case ErrorCodeCategory::Win32:
					{
						return Win32Error(m_Value).IsSuccess();
					}
					case ErrorCodeCategory::NtStatus:
					{
						return NtStatus(m_Value).IsSuccess();
					}
					case ErrorCodeCategory::HResult:
					{
						return HResult(m_Value).IsSuccess();
					}
				};
				return false;
			}
			constexpr bool IsFail() const noexcept
			{
				return IsKnown() && !IsSuccess();
			}

			String ToString() const
			{
				switch (m_Category)
				{
					case ErrorCodeCategory::Win32:
					{
						return Win32Error(m_Value).ToString();
					}
					case ErrorCodeCategory::NtStatus:
					{
						return NtStatus(m_Value).ToString();
					}
					case ErrorCodeCategory::HResult:
					{
						return HResult(m_Value).ToString();
					}
				};
				return {};
			}
			String GetMessage() const
			{
				switch (m_Category)
				{
					case ErrorCodeCategory::Win32:
					{
						return Win32Error(m_Value).GetMessage();
					}
					case ErrorCodeCategory::NtStatus:
					{
						return NtStatus(m_Value).GetMessage();
					}
					case ErrorCodeCategory::HResult:
					{
						return HResult(m_Value).GetMessage();
					}
				};
				return {};
			}

		public:
			constexpr explicit operator bool() const noexcept
			{
				return IsSuccess();
			}
			constexpr bool operator!() const noexcept
			{
				return !IsSuccess();
			}

			constexpr bool operator==(const ErrorCode& other) const noexcept
			{
				return m_Value == other.m_Value && m_Category == other.m_Category;
			}
			constexpr bool operator==(GenericError other) const noexcept
			{
				return IsEqualValue(other);
			}
			constexpr bool operator==(Win32Error other) const noexcept
			{
				return IsEqualValue(other);
			}
			constexpr bool operator==(NtStatus other) const noexcept
			{
				return IsEqualValue(other);
			}
			constexpr bool operator==(HResult other) const noexcept
			{
				return IsEqualValue(other);
			}

			template<class T>
			constexpr bool operator!=(T&& other) const noexcept
			{
				return !(*this == other);
			}

			constexpr ErrorCode& operator=(ErrorCode&& other) noexcept
			{
				constexpr ErrorCode null;

				m_Value = other.m_Value;
				m_Category = other.m_Category;

				other.m_Value = null.m_Value;
				other.m_Category = null.m_Category;

				return *this;
			}
			constexpr ErrorCode& operator=(const ErrorCode&) noexcept = default;
	};
}
