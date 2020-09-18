#include "stdafx.h"
#include "ErrorCode.h"

namespace kxf
{
	bool ErrorCode::IsSameAs(const ErrorCode& other) const noexcept
	{
		if (this == &other || IsNull() && other.IsNull())
		{
			return true;
		}
		else if (m_ErrorCode && other.m_ErrorCode && m_InterfaceID == other.m_InterfaceID)
		{
			return m_ErrorCode->GetValue() == other.m_ErrorCode->GetValue();
		}
		return false;
	}
}
