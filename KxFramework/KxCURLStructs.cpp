#include "KxStdAfx.h"
#include "KxFramework/KxCURLStructs.h"
#include "KxFramework/KxCURL.h"

wxString KxCURLReplyBase::GetErrorMessage() const
{
	if (!m_ErrorMessage.IsEmpty() || m_ErrorCode == ErrorCode::Invalid)
	{
		return m_ErrorMessage;
	}
	else
	{
		return KxCURL::GetInstance().ErrorCodeToString(m_ErrorCode);
	}
}
