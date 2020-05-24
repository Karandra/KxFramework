#pragma once
#include "Common.h"
#include "CURL/Session.h"
#include "CURL/Event.h"
#include "CURL/Reply.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"

namespace kxf::CURL
{
	String GetLibraryName();
	Version GetLibraryVersion();

	String ErrorCodeToString(int code);
}
