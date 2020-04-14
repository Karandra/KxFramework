#pragma once
#include "Common.h"
#include "CURLSession.h"
#include "CURLEvent.h"
#include "CURLReply.h"
#include "Kx/General/String.h"
#include "Kx/General/Version.h"

namespace KxFramework::CURL
{
	String GetLibraryName();
	Version GetLibraryVersion();

	String ErrorCodeToString(int code);
}
