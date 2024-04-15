#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"
#include "kxf/Core/Version.h"

namespace kxf::Crypto
{
	KX_API String GetLibraryName();
	KX_API Version GetLibraryVersion();
}
