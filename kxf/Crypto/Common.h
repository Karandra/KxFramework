#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"

namespace kxf::Crypto
{
	KX_API String GetLibraryName();
	KX_API Version GetLibraryVersion();
}
