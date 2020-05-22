#pragma once
#include "Kx/Common.hpp"
#include "Kx/General/String.h"
#include "Kx/General/Version.h"
#include "Kx/Drawing/Geometry.h"
#include "CommonDefs.h"

namespace KxFramework::Sciter
{
	String GetLibraryName();
	Version GetLibraryVersion();

	bool IsLibraryLoaded();
	bool LoadLibrary(const String& path);
	void FreeLibrary();
}

namespace KxFramework::Sciter
{
	String SizeUnitToString(SizeUnit unit);
	
	Alignment MapAlignment(wxAlignment alignment);
	wxAlignment MapAlignment(Alignment alignment);

	bool SetMasterCSS(const String& css);
	bool AppendMasterCSS(const String& css);
}
