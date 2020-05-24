#include "stdafx.h"
#include "Common.h"
#include <wx/stream.h> 
#include <wx/filefn.h> 

namespace kxf
{
	std::optional<wxSeekMode> ToWxSeekMode(StreamSeekMode seekMode) noexcept
	{
		switch (seekMode)
		{
			case StreamSeekMode::FromStart:
			{
				return wxSeekMode::wxFromStart;
			}
			case StreamSeekMode::FromCurrent:
			{
				return wxSeekMode::wxFromCurrent;
			}
			case StreamSeekMode::FromEnd:
			{
				return wxSeekMode::wxFromEnd;
			}
		};
		return {};
	}
	std::optional<StreamSeekMode> FromWxSeekMode(wxSeekMode seekMode) noexcept
	{
		switch (seekMode)
		{
			case wxSeekMode::wxFromStart:
			{
				return StreamSeekMode::FromStart;
			}
			case wxSeekMode::wxFromCurrent:
			{
				return StreamSeekMode::FromCurrent;
			}
			case wxSeekMode::wxFromEnd:
			{
				return StreamSeekMode::FromEnd;
			}
		};
		return {};
	}

	std::optional<wxStreamError> ToWxStreamError(StreamErrorCode streamError) noexcept
	{
		switch (streamError)
		{
			case StreamErrorCode::Success:
			{
				return wxStreamError::wxSTREAM_NO_ERROR;
			}
			case StreamErrorCode::EndOfStream:
			{
				return wxStreamError::wxSTREAM_EOF;
			}
			case StreamErrorCode::ReadError:
			{
				return wxStreamError::wxSTREAM_READ_ERROR;
			}
			case StreamErrorCode::WriteError:
			{
				return wxStreamError::wxSTREAM_WRITE_ERROR;
			}
		};
		return {};
	}
	std::optional<StreamErrorCode> FromWxStreamError(wxStreamError streamError) noexcept
	{
		switch (streamError)
		{
			case wxStreamError::wxSTREAM_NO_ERROR:
			{
				return StreamErrorCode::Success;
			}
			case wxStreamError::wxSTREAM_EOF:
			{
				return StreamErrorCode::EndOfStream;
			}
			case wxStreamError::wxSTREAM_READ_ERROR:
			{
				return StreamErrorCode::ReadError;
			}
			case wxStreamError::wxSTREAM_WRITE_ERROR:
			{
				return StreamErrorCode::WriteError;
			}
		};
		return {};
	}
}
