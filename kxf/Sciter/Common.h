#pragma once
#include "kxf/Common.hpp"
#include "kxf/UI/Common.h"
#include "kxf/General/Common.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"
#include "kxf/General/ResourceID.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Network/URI.h"
#include "kxf/Network/URL.h"
#include "CommonDefs.h"

namespace kxf::Sciter
{
	String GetLibraryName();
	Version GetLibraryVersion();

	bool IsLibraryLoaded();
	bool LoadLibrary(const String& path);
	void FreeLibrary();
}

namespace kxf::Sciter
{
	String SizeUnitToString(SizeUnit unit);
	
	CornerAlignment MapCornerAlignment(FlagSet<Alignment> alignment);
	FlagSet<Alignment> MapCornerAlignment(CornerAlignment alignment);
}
