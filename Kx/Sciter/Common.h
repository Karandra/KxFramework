#pragma once
#include "Kx/Common.hpp"
#include <KxFramework/KxEnumClassOperations.h>
#include <KxFramework/KxVersion.h>
#include "CommonDefs.h"

namespace KxSciter
{
	wxString GetLibraryName();
	KxVersion GetLibraryVersion();

	bool IsLibraryLoaded();
	bool LoadLibrary(const wxString& path);
	void FreeLibrary();
}

namespace KxSciter
{
	wxStringView SizeUnitToString(SizeUnit unit);
}