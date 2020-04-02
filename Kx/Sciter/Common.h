#pragma once
#include "Kx/Common.hpp"
#include <KxFramework/KxVersion.h>
#include "CommonDefs.h"

namespace KxFramework::Sciter
{
	wxString GetLibraryName();
	KxVersion GetLibraryVersion();

	bool IsLibraryLoaded();
	bool LoadLibrary(const wxString& path);
	void FreeLibrary();
}

namespace KxFramework::Sciter
{
	wxStringView SizeUnitToString(SizeUnit unit);
	
	Alignment MapAlignment(wxAlignment alignment);
	wxAlignment MapAlignment(Alignment alignment);

	bool SetMasterCSS(const wxString& css);
	bool AppendMasterCSS(const wxString& css);
}
