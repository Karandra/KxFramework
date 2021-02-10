#pragma once
#include "Common.h"
#include "kxf/General/IErrorCode.h"

namespace kxf
{
	class KX_API ErrorCode final: public RTTI::Implementation<ErrorCode, IErrorCode>
	{
		private:
			std::unique_ptr<IErrorCode> m_ErrorCode;
			IID m_IID;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				if (m_ErrorCode && iid == m_IID)
				{
					return m_ErrorCode->QueryInterface(iid);
				}
				return Implementation::DoQueryInterface(iid);
			}
			
		public:
			ErrorCode() noexcept = default;

			template<class T, class = std::enable_if_t<std::is_base_of_v<IErrorCode, T>>>
			ErrorCode(T errorCode) noexcept
				:m_ErrorCode(std::make_unique<T>(std::move(errorCode))), m_IID(RTTI::GetInterfaceID<T>())
			{
			}

			ErrorCode(std::unique_ptr<IErrorCode> errorCode) noexcept
				:m_ErrorCode(std::move(errorCode))
			{
				if (m_ErrorCode)
				{
					m_IID = m_ErrorCode->QueryInterface<RTTI::ClassInfo>()->GetIID();
				}
			}

			ErrorCode(ErrorCode&&) noexcept = default;
			ErrorCode(const ErrorCode&) = delete;

		public:
			// IErrorCode
			bool IsSuccess() const noexcept override
			{
				return m_ErrorCode && m_ErrorCode->IsSuccess();
			}
			bool IsFail() const noexcept override
			{
				return m_ErrorCode && m_ErrorCode->IsFail();
			}

			uint32_t GetValue() const noexcept override
			{
				return m_ErrorCode ? m_ErrorCode->GetValue() : std::numeric_limits<uint32_t>::max();
			}
			void SetValue(uint32_t value) noexcept override
			{
				if (m_ErrorCode)
				{
					m_ErrorCode->SetValue(value);
				}
			}

			String ToString() const override
			{
				if (m_ErrorCode)
				{
					return m_ErrorCode->ToString();
				}
				return {};
			}
			String GetMessage(const Locale& locale = {}) const
			{
				if (m_ErrorCode)
				{
					return m_ErrorCode->GetMessage(locale);
				}
				return {};
			}

			// ErrorCode
			bool IsNull() const noexcept
			{
				return m_ErrorCode == nullptr || m_IID.IsNull();
			}
			bool IsSameAs(const ErrorCode& other) const noexcept;

			template<class TErrorCode, class... Args, class = std::enable_if_t<std::is_base_of_v<IErrorCode, TErrorCode> && std::is_constructible_v<TErrorCode, Args...>>>
			bool IsSameAs(Args&&... arg) const noexcept
			{
				TErrorCode errorCode(std::forward<Args>(arg)...);
				return IsSameAs(errorCode);
			}

		public:
			bool operator==(const ErrorCode& other) const noexcept
			{
				return IsSameAs(other);
			}
			bool operator!=(const ErrorCode& other) const noexcept
			{
				return !IsSameAs(other);
			}

			ErrorCode& operator=(ErrorCode&&) noexcept = default;
			ErrorCode& operator=(const ErrorCode&) = delete;
	};
}
