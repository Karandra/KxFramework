#pragma once
#include <kxf/Common.hpp>
#include "kxf/General/String.h"
#include "kxf/General/BinarySize.h"
enum wxSeekMode: int;
enum wxStreamError: int;

namespace kxf
{
	using StreamOffset = BinarySize;

	enum class IOStreamAccess: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		ReadAttributes = 1 << 2,
		WriteAttributes = 1 << 3,

		RW = Read|Write,
		AllAccess = RW|ReadAttributes|WriteAttributes
	};
	KxFlagSet_Declare(IOStreamAccess);

	enum class IOStreamShare: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Delete = 1 << 2,

		Everything = Read|Write|Delete
	};
	KxFlagSet_Declare(IOStreamShare);

	enum class IOStreamFlag: uint32_t
	{
		None = 0,

		Normal = 1 << 0,
		AllowDirectories = 1 << 1
	};
	KxFlagSet_Declare(IOStreamFlag);

	enum class IOStreamSeek
	{
		FromStart,
		FromCurrent,
		FromEnd,
	};
	enum class IOStreamDisposition
	{
		OpenExisting,
		OpenAlways,
		CreateNew,
		CreateAlways,
	};
}

namespace kxf::IO
{
	KX_API std::optional<wxSeekMode> ToWxSeekMode(IOStreamSeek seekMode) noexcept;
	KX_API std::optional<IOStreamSeek> FromWxSeekMode(wxSeekMode seekMode) noexcept;
}
