#pragma once
#include "Common.h"
#include "CURL/Session.h"
#include "CURL/Event.h"
#include "CURL/Reply.h"
#include "Kx/General/String.h"
#include "Kx/General/Version.h"

namespace KxFramework::CURL
{
	String GetLibraryName();
	Version GetLibraryVersion();

	String ErrorCodeToString(int code);
}
