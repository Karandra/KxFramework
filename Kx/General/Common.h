#pragma once
#include <Kx/Common.hpp>
#include "Kx/General/EnumClass.h"

namespace KxFramework
{
	enum class VersionType
	{
		None = -1,

		Default,
		DateTime,
	};
	enum class StreamSeekMode
	{
		FromStart = wxSeekMode::wxFromStart,
		FromCurrent = wxSeekMode::wxFromCurrent,
		FromEnd = wxSeekMode::wxFromEnd,
	};
	enum class StreamErrorCode
	{
		Success = wxSTREAM_NO_ERROR,
		ReadError = wxSTREAM_WRITE_ERROR,
		WriteError = wxSTREAM_READ_ERROR,
		EndOfstream = wxSTREAM_EOF
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowCast(VersionType);
		Kx_EnumClass_AllowCast(StreamSeekMode);
		Kx_EnumClass_AllowCast(StreamErrorCode);
	}
}
