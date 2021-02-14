#pragma once
#include "Common.h"
#include "CURL/Session.h"
#include "CURL/Event.h"
#include "CURL/Reply.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"

namespace kxf::CURL
{
	KX_API String GetLibraryName();
	KX_API Version GetLibraryVersion();

	KX_API String ErrorCodeToString(int code);
}
