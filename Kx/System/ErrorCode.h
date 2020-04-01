#pragma once
#include "Common.h"
#include "ErrorCodeValue.h"

namespace KxFramework
{
	class KX_API ErrorCode final
	{
		private:
			int64_t m_Value = -1;
			ErrorCodeCategory m_Category = ErrorCodeCategory::Unknown;

		private:
			template<class T>
			ErrorCode& AssignCode(T errorCode) noexcept
			{
				m_Value = static_cast<int64_t>(errorCode.GetValue());
				m_Category = T::GetCategory();

				return *this;
			}

			template<class T>
			bool IsEqualValue(T errorCode) const noexcept
			{
				return m_Category == T::GetCategory() && static_cast<typename T::TValueType>(m_Value) == errorCode.GetValue();
			}

			template<class T>
			std::optional<T> GetAsCode() const noexcept
			{
				if (m_Category == T::GetCategory())
				{
					return T(static_cast<typename T::TValueType>(m_Value));
				}
				return {};
			}

		public:
			constexpr ErrorCode() noexcept = default;
			ErrorCode(GenericErrorCode errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			ErrorCode(Win32ErrorCode errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			ErrorCode(NtStatusCode errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			ErrorCode(HRESULTCode errorCode) noexcept
			{
				AssignCode(errorCode);
			}
			ErrorCode(ErrorCode&& other) noexcept
			{
				*this = std::move(other);
			}
			ErrorCode(const ErrorCode&) noexcept = default;

		public:
			ErrorCodeCategory GetCategory() const noexcept
			{
				return m_Category;
			}
			std::optional<GenericErrorCode> GetGeneric() const noexcept
			{
				return GetAsCode<GenericErrorCode>();
			}
			std::optional<Win32ErrorCode> GetWin32() const noexcept
			{
				return GetAsCode<Win32ErrorCode>();
			}
			std::optional<NtStatusCode> GetNtStatus() const noexcept
			{
				return GetAsCode<NtStatusCode>();
			}
			std::optional<HRESULTCode> GetHRESULT() const noexcept
			{
				return GetAsCode<HRESULTCode>();
			}

			std::optional<Win32ErrorCode> ConvertToWin32() const noexcept;
			std::optional<NtStatusCode> ConvertToNtStatus() const noexcept;
			std::optional<HRESULTCode> ConvertToHRESULT() const noexcept;

			bool IsSuccessful() const noexcept
			{
				switch (m_Category)
				{
					case ErrorCodeCategory::Generic:
					{
						return GenericErrorCode(m_Value).IsSuccessful();
					}
					case ErrorCodeCategory::Win32:
					{
						return Win32ErrorCode(m_Value).IsSuccessful();
					}
					case ErrorCodeCategory::NtStatus:
					{
						return NtStatusCode(m_Value).IsSuccessful();
					}
					case ErrorCodeCategory::HRESULT:
					{
						return HRESULTCode(m_Value).IsSuccessful();
					}
				};
				return false;
			}
			bool IsFailed() const noexcept
			{
				return IsKnown() && !IsSuccessful();
			}
			bool IsKnown() const noexcept
			{
				return m_Category != ErrorCodeCategory::Unknown;
			}

		public:
			explicit operator bool() const noexcept
			{
				return IsSuccessful();
			}
			bool operator!() const noexcept
			{
				return !IsSuccessful();
			}

			bool operator==(const ErrorCode& other) const noexcept
			{
				return m_Value == other.m_Value && m_Category == other.m_Category;
			}
			bool operator==(GenericErrorCode other) const noexcept
			{
				return IsEqualValue(other);
			}
			bool operator==(Win32ErrorCode other) const noexcept
			{
				return IsEqualValue(other);
			}
			bool operator==(NtStatusCode other) const noexcept
			{
				return IsEqualValue(other);
			}
			bool operator==(HRESULTCode other) const noexcept
			{
				return IsEqualValue(other);
			}

			template<class T>
			bool operator!=(T&& other) const noexcept
			{
				return !(*this == other);
			}

			ErrorCode& operator=(ErrorCode&& other) noexcept
			{
				constexpr ErrorCode null;

				m_Value = other.m_Value;
				m_Category = other.m_Category;

				other.m_Value = null.m_Value;
				other.m_Category = null.m_Category;

				return *this;
			}
			ErrorCode& operator=(const ErrorCode&) noexcept = default;
	};
}
