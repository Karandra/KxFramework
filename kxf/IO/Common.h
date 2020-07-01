#pragma once
#include <kxf/Common.hpp>
#include "kxf/General/String.h"
#include "kxf/General/BinarySize.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
enum wxSeekMode: int;
enum wxStreamError: int;

namespace kxf
{
	enum class StreamSeekMode
	{
		FromStart,
		FromCurrent,
		FromEnd,
	};
	enum class StreamErrorCode
	{
		Success,
		EndOfStream,
		ReadError,
		WriteError
	};
}

namespace kxf
{
	std::optional<wxSeekMode> ToWxSeekMode(StreamSeekMode seekMode) noexcept;
	std::optional<StreamSeekMode> FromWxSeekMode(wxSeekMode seekMode) noexcept;

	std::optional<wxStreamError> ToWxStreamError(StreamErrorCode streamError) noexcept;
	std::optional<StreamErrorCode> FromWxStreamError(wxStreamError streamError) noexcept;
}
