#pragma once
#include "Common.h"
#include "kxf/General/IErrorCode.h"

namespace kxf
{
	class KX_API ErrorCode final: public RTTI::ImplementInterface<ErrorCode, IErrorCode>
	{
		private:
			std::unique_ptr<IErrorCode> m_ErrorCode;
			IID m_InterfaceID;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				if (m_ErrorCode && iid == m_InterfaceID)
				{
					return m_ErrorCode->QueryInterface(iid);
				}
				return ImplementInterface::DoQueryInterface(iid);
			}
			
		public:
			ErrorCode() noexcept = default;

			template<class T, class = std::enable_if_t<std::is_base_of_v<IErrorCode, T>>>
			ErrorCode(T errorCode) noexcept
				:m_ErrorCode(std::move(errorCode)), m_InterfaceID(RTTI::GetInterfaceID<T>())
			{
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
				return m_ErrorCode == nullptr || m_InterfaceID.IsNull();
			}
			bool IsSameAs(const ErrorCode& other) const noexcept;

		public:
			bool operator==(const ErrorCode& other) const noexcept
			{
				return IsSameAs(other);
			}
			bool operator!=(const ErrorCode& other) const noexcept
			{
				return IsSameAs(other);
			}

			ErrorCode& operator=(ErrorCode&&) noexcept = default;
			ErrorCode& operator=(const ErrorCode&) = delete;
	};
}
