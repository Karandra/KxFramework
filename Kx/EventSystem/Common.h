#pragma once
#include "Kx/Common.hpp"
#include <wx/event.h>

namespace KxFramework
{
	using EventID = wxEventType;
	
	template<class T>
	using EventTag = wxEventTypeTag<T>;
}
