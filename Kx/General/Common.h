#pragma once
#include <Kx/Common.hpp>
#include "Kx/General/EnumClass.h"
enum wxSeekMode: int;
enum wxStreamError: int;

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
	enum class UnitNameFlag
	{
		None = 0,

		Abbreviated = 1 << 0,
		GenetiveCase = 1 << 1,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowCast(VersionType);
		Kx_EnumClass_AllowEverything(UnitNameFlag);
	}
}

namespace KxFramework
{
	std::optional<wxSeekMode> ToWxSeekMode(StreamSeekMode seekMode) noexcept;
	std::optional<StreamSeekMode> FromWxSeekMode(wxSeekMode seekMode) noexcept;

	std::optional<wxStreamError> ToWxStreamError(StreamErrorCode streamError) noexcept;
	std::optional<StreamErrorCode> FromWxStreamError(wxStreamError streamError) noexcept;
}
