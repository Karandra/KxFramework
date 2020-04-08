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
			constexpr ErrorCode(GenericErrorCode errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			constexpr ErrorCode(Win32ErrorCode errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			constexpr ErrorCode(NtStatusCode errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			constexpr ErrorCode(HResultCode errorCode) noexcept
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
			constexpr std::optional<GenericErrorCode> GetGeneric() const noexcept
			{
				return GetAsCode<GenericErrorCode>();
			}
			constexpr std::optional<Win32ErrorCode> GetWin32() const noexcept
			{
				return GetAsCode<Win32ErrorCode>();
			}
			constexpr std::optional<NtStatusCode> GetNtStatus() const noexcept
			{
				return GetAsCode<NtStatusCode>();
			}
			constexpr std::optional<HResultCode> GetHResult() const noexcept
			{
				return GetAsCode<HResultCode>();
			}

			std::optional<Win32ErrorCode> ConvertToWin32() const noexcept;
			std::optional<NtStatusCode> ConvertToNtStatus() const noexcept;
			std::optional<HResultCode> ConvertToHResult() const noexcept;

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
						return GenericErrorCode(m_Value).IsSuccess();
					}
					case ErrorCodeCategory::Win32:
					{
						return Win32ErrorCode(m_Value).IsSuccess();
					}
					case ErrorCodeCategory::NtStatus:
					{
						return NtStatusCode(m_Value).IsSuccess();
					}
					case ErrorCodeCategory::HResult:
					{
						return HResultCode(m_Value).IsSuccess();
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
						return Win32ErrorCode(m_Value).ToString();
					}
					case ErrorCodeCategory::NtStatus:
					{
						return NtStatusCode(m_Value).ToString();
					}
					case ErrorCodeCategory::HResult:
					{
						return HResultCode(m_Value).ToString();
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
						return Win32ErrorCode(m_Value).GetMessage();
					}
					case ErrorCodeCategory::NtStatus:
					{
						return NtStatusCode(m_Value).GetMessage();
					}
					case ErrorCodeCategory::HResult:
					{
						return HResultCode(m_Value).GetMessage();
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
			constexpr bool operator==(GenericErrorCode other) const noexcept
			{
				return IsEqualValue(other);
			}
			constexpr bool operator==(Win32ErrorCode other) const noexcept
			{
				return IsEqualValue(other);
			}
			constexpr bool operator==(NtStatusCode other) const noexcept
			{
				return IsEqualValue(other);
			}
			constexpr bool operator==(HResultCode other) const noexcept
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
