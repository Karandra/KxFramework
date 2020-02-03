#include "KxStdAfx.h"
#include "Common.h"
#include "SciterAPI.h"
#include <KxFramework/KxLibrary.h>

namespace KxSciter
{
	void* g_SciterLibrary = nullptr;

	wxString GetLibraryName()
	{
		return wxS("Sciter");
	}
	KxVersion GetLibraryVersion()
	{
		if (KxLibrary library(reinterpret_cast<HMODULE>(g_SciterLibrary)); library.IsOK())
		{
			KxLibraryVersionInfo versionInfo = KxLibrary::GetVersionInfoFromFile(library.GetFileName());
			if (versionInfo.IsOK())
			{
				return versionInfo.GetString(wxS("ProductVersionString"));
			}
		}
		return {};
	}

	bool IsLibraryLoaded()
	{
		return g_SciterLibrary != nullptr;
	}
	bool LoadLibrary(const wxString& path)
	{
		if (!g_SciterLibrary)
		{
			g_SciterLibrary = ::LoadLibraryW(path.wc_str());
		}
		return g_SciterLibrary != nullptr;
	}
	void FreeLibrary()
	{
		// Sciter API included don't have function to unload its DLL and it causes a crash
		// if I do unload the DLL. So I'm going to let it loaded. It seems Sciter does something
		// in its DllMain's 'DLL_PROCESS_DETACH' event.
		#if 0
		if (g_SciterLibrary)
		{
			::FreeLibrary(reinterpret_cast<HMODULE>(g_SciterLibrary));
			g_SciterLibrary = nullptr;
		}
		#endif
	}
}

namespace KxSciter
{
	wxStringView SizeUnitToString(SizeUnit unit)
	{
		#define Map(unit)	\
			case SizeUnit::##unit:	\
			{	\
				return wxS(#unit);	\
			}	\

		switch (unit)
		{
			// Absolute
			Map(cm);
			Map(mm);
			Map(in);
			Map(px);
			Map(pt);
			Map(pc);
			Map(dip);

			// Relative
			Map(em);
			Map(ex);
			Map(ch);
			Map(rem);
			Map(vw);
			Map(vh);
			Map(vmin);
			Map(vmax);

			case SizeUnit::Percent:
			{
				return wxS("%");
			}
		};
		return {};

		#undef Map
	}
}
