#pragma once
#include "Common.h"

namespace KxFramework
{
	enum class ErrorCodeCategory
	{
		Any = -2,
		Unknown = -1,

		Generic,
		Win32,
		NtStatus,
		HRESULT
	};
}

namespace KxFramework
{
	class KX_API ErrorCode final
	{
		public:
			static ErrorCode FromGeneric(int64_t code) noexcept
			{
				return ErrorCode().AssignCode(code, ErrorCodeCategory::Generic);
			}
			static ErrorCode FromUnknown(int64_t code) noexcept
			{
				return ErrorCode().AssignCode(code, ErrorCodeCategory::Unknown);
			}
			static ErrorCode FromWin32(int64_t code) noexcept
			{
				return ErrorCode().AssignCode(code, ErrorCodeCategory::Win32);
			}
			static ErrorCode FromHRESULT(int64_t code) noexcept
			{
				return ErrorCode().AssignCode(code, ErrorCodeCategory::HRESULT);
			}
			static ErrorCode FromNtStatus(int64_t code) noexcept
			{
				return ErrorCode().AssignCode(code, ErrorCodeCategory::NtStatus);
			}

		private:
			std::optional<int64_t> m_Value;
			ErrorCodeCategory m_Category = ErrorCodeCategory::Unknown;

		private:
			ErrorCode& AssignCode(int64_t code, ErrorCodeCategory category) noexcept
			{
				m_Value = code;
				category = category;
			}

		public:
			ErrorCode() noexcept = default;
			ErrorCode(ErrorCode&&) noexcept = default;
			ErrorCode(const ErrorCode&) noexcept = default;

		public:
			template<ErrorCodeCategory category = ErrorCodeCategory::Any>
			std::optional<int64_t> GetValue() const noexcept
			{
				if constexpr(category == ErrorCodeCategory::Win32)
				{
					return GetWin32();
				}
				else if constexpr(category == ErrorCodeCategory::NtStatus)
				{
					return GetHRESULT();
				}
				else if constexpr(category == ErrorCodeCategory::HRESULT)
				{
					return GetHRESULT();
				}
				else if constexpr(category == ErrorCodeCategory::Generic)
				{
					return GetGeneric();
				}
				else if constexpr(category == ErrorCodeCategory::Unknown)
				{
					return GetUnknown();
				}
				else
				{
					return m_Value;
				}
			}

			ErrorCodeCategory GetCategory() const noexcept
			{
				return m_Category;
			}

			std::optional<int64_t> GetGeneric() const noexcept
			{
				return m_Category == ErrorCodeCategory::Generic ? m_Value : std::nullopt;
			}
			std::optional<int64_t> GetUnknown() const noexcept
			{
				return m_Category == ErrorCodeCategory::Unknown ? m_Value : std::nullopt;
			}
			std::optional<int64_t> GetWin32() const noexcept
			{
				return m_Category == ErrorCodeCategory::Win32 ? m_Value : std::nullopt;
			}
			std::optional<int64_t> GetNtStatus() const noexcept
			{
				return m_Category == ErrorCodeCategory::NtStatus ? m_Value : std::nullopt;
			}
			std::optional<int64_t> GetHRESULT() const noexcept
			{
				return m_Category == ErrorCodeCategory::HRESULT ? m_Value : std::nullopt;
			}

			std::optional<int64_t> ConvertToWin32() const noexcept;
			std::optional<int64_t> ConvertToNtStatus() const noexcept;
			std::optional<int64_t> ConvertToHRESULT() const noexcept;

			template<ErrorCodeCategory category>
			bool IsSameAs(int64_t value) const noexcept
			{
				return GetValue<category>() == value;
			}

			bool IsEmpty() const noexcept
			{
				return !m_Value.has_value();
			}
			bool IsSuccessful() const noexcept;
			bool IsFailed() const noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return IsSuccessful();
			}
			bool operator!() const noexcept
			{
				return !IsSuccessful();
			}

			ErrorCode& operator=(ErrorCode&&) noexcept = default;
			ErrorCode& operator=(const ErrorCode&) noexcept = default;
	};
}
