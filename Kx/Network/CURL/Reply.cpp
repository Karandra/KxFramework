#include "stdafx.h"
#include "Reply.h"
#include "../CURL.h"

namespace KxFramework
{
	String CURLReplyBase::GetErrorMessage() const
	{
		if (!m_ErrorMessage.IsEmpty() || m_ErrorCode == static_cast<int>(ErrorCode::Invalid))
		{
			return m_ErrorMessage;
		}
		else
		{
			return CURL::ErrorCodeToString(m_ErrorCode);
		}
	}
}
