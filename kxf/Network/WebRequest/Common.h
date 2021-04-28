#pragma once
#include "../Common.h"
#include "../URI.h"
#include "../HTTPStatus.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/IO/Common.h"
#include "kxf/General/String.h"
#include "kxf/General/FlagSet.h"
#include "kxf/General/DateTime.h"
#include "kxf/General/BinarySize.h"
#include "kxf/General/Enumerator.h"

namespace kxf
{
	class FSPath;
	class IFileSystem;

	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	enum class WebRequestState
	{
		None = -1,

		Idle,
		Paused,
		Resumed,
		Unauthorized,
		Active,
		Completed,
		Failed,
		Cancelled
	};
	enum class WebRequestStorage
	{
		None = -1,

		Memory,
		Stream,
		FileSystem
	};

	enum class WebRequestHeaderFlag: uint32_t
	{
		None = 0,

		Add = 1 << 0,
		Replace = 1 << 1,

		CoalesceComma = 1 << 16,
		CoalesceSemicolon = 1 << 17
	};
	KxFlagSet_Declare(WebRequestHeaderFlag);
}
