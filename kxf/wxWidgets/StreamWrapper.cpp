#include "stdafx.h"
#include "StreamWrapper.h"

namespace
{
	kxf::Win32Error TranslateErrorCode(const wxStreamBase& stream)
	{
		using namespace kxf;

		if (stream.IsOk())
		{
			switch (stream.GetLastError())
			{
				case wxStreamError::wxSTREAM_NO_ERROR:
				{
					return Win32Error::Success();
				}
				case wxStreamError::wxSTREAM_EOF:
				{
					return ERROR_HANDLE_EOF;
				}
				case wxStreamError::wxSTREAM_READ_ERROR:
				{
					return ERROR_READ_FAULT;
				}
				case wxStreamError::wxSTREAM_WRITE_ERROR:
				{
					return ERROR_WRITE_FAULT;
				}
			};
		}
		return Win32Error::Fail();
	}
}

namespace kxf::wxWidgets
{
	ErrorCode InputStreamWrapper::GetLastError() const
	{
		return m_LastError ? m_LastError : TranslateErrorCode(*m_Stream);
	}
}

namespace kxf::wxWidgets
{
	ErrorCode OutputStreamWrapper::GetLastError() const
	{
		return m_LastError ? m_LastError : TranslateErrorCode(*m_Stream);
	}
}
