#pragma once
#include "../Common.h"
#include "../URI.h"
#include "../HTTPStatus.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/IO/Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/FlagSet.h"
#include "kxf/Core/DateTime.h"
#include "kxf/Core/BinarySize.h"
#include "kxf/Core/Enumerator.h"

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
		Started,
		Active,
		Unauthorized,
		Completed,
		Cancelled,
		Failed
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
