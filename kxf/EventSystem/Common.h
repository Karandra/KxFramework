#pragma once
#include "kxf/Common.hpp"
#include <wx/event.h>
#include <wx/windowid.h>

namespace kxf
{
	enum class EventCategory: uint32_t
	{
		None = 0,

		UI = wxEVT_CATEGORY_UI,
		Timer = wxEVT_CATEGORY_TIMER,
		Thread = wxEVT_CATEGORY_THREAD,
		Socket = wxEVT_CATEGORY_SOCKET,
		Unknown = wxEVT_CATEGORY_UNKNOWN,
		Clipboard = wxEVT_CATEGORY_CLIPBOARD,
		UserInput = wxEVT_CATEGORY_USER_INPUT,

		Everything = UI|Timer|Thread|Socket|Unknown|Clipboard|UserInput
	};
	KxDeclareFlagSet(EventCategory);

	enum class EventFlag: uint32_t
	{
		None = 0,

		Direct = 1,
		Queued = 2,

		Unique = 1 << 16,
		Blocking = 1 << 17,
		AlwaysSkip = 1 << 18,
		OneShot = 1 << 19
	};
	KxDeclareFlagSet(EventFlag);

	enum class SignalParametersSemantics
	{
		Move,
		Copy
	};
}
