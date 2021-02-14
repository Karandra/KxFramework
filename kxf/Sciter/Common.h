#pragma once
#include "kxf/Common.hpp"
#include "kxf/UI/Common.h"
#include "kxf/General/Common.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"
#include "kxf/General/DateTime.h"
#include "kxf/General/ResourceID.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/Angle.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Network/URI.h"
#include "kxf/Network/URL.h"
#include "CommonDefs.h"

namespace kxf::Sciter
{
	KX_API String GetLibraryName();
	KX_API Version GetLibraryVersion();

	KX_API bool IsLibraryLoaded();
	KX_API bool LoadLibrary(const String& path);
	KX_API void FreeLibrary();
}

namespace kxf::Sciter
{
	KX_API String SizeUnitToString(SizeUnit unit);

	KX_API CornerAlignment MapCornerAlignment(FlagSet<Alignment> alignment);
	KX_API FlagSet<Alignment> MapCornerAlignment(CornerAlignment alignment);
}
