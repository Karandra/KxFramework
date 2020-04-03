#include "KxStdAfx.h"
#include "Common.h"
#include "SciterAPI.h"
#include "Internal.h"
#include <KxFramework/KxLibrary.h>

namespace KxFramework::Sciter
{
	void* g_SciterLibrary = nullptr;

	wxString GetLibraryName()
	{
		return wxS("Sciter");
	}
	Version GetLibraryVersion()
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

namespace KxFramework::Sciter
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

			case SizeUnit::pr:
			{
				return wxS("%");
			}
			case SizeUnit::sp:
			{
				return wxS("%%");
			}
		};
		return {};

		#undef Map
	}

	Alignment MapAlignment(wxAlignment alignment)
	{
		if (alignment == wxALIGN_INVALID)
		{
			return Alignment::Unspecified;
		}
		else if (alignment & wxALIGN_RIGHT)
		{
			// Right alignment
			if (alignment & wxALIGN_CENTER_VERTICAL)
			{
				return Alignment::MiddleRight;
			}
			else if (alignment & wxALIGN_BOTTOM)
			{
				return Alignment::BottomRight;
			}
			else
			{
				return Alignment::TopRight;
			}
		}
		else if (alignment & wxALIGN_CENTRE_HORIZONTAL)
		{
			// Horizontally-centered
			if (alignment & wxALIGN_CENTER_VERTICAL)
			{
				return Alignment::MiddleCenter;
			}
			else if (alignment & wxALIGN_BOTTOM)
			{
				return Alignment::BottomCenter;
			}
			else
			{
				return Alignment::TopCenter;
			}
		}
		else
		{
			// Left aligned
			if (alignment & wxALIGN_CENTER_VERTICAL)
			{
				return Alignment::MiddleLeft;
			}
			else if (alignment & wxALIGN_BOTTOM)
			{
				return Alignment::BottomLeft;
			}
			else
			{
				return Alignment::TopLeft;
			}
		}
		return Alignment::Unspecified;
	}
	wxAlignment MapAlignment(Alignment alignment)
	{
		switch (alignment)
		{
			case Alignment::TopLeft:
			{
				return wxALIGN_LEFT;
			}
			case Alignment::TopCenter:
			{
				return wxALIGN_CENTER_HORIZONTAL;
			}
			case Alignment::TopRight:
			{
				return wxALIGN_RIGHT;
			}

			case Alignment::MiddleLeft:
			{
				return wxALIGN_CENTER_VERTICAL;
			}
			case Alignment::MiddleCenter:
			{
				return static_cast<wxAlignment>(wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL);
			}
			case Alignment::MiddleRight:
			{
				return static_cast<wxAlignment>(wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);
			}

			case Alignment::BottomLeft:
			{
				return wxALIGN_BOTTOM;
			}
			case Alignment::BottomCenter:
			{
				return static_cast<wxAlignment>(wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL);
			}
			case Alignment::BottomRight:
			{
				return static_cast<wxAlignment>(wxALIGN_BOTTOM|wxALIGN_RIGHT);
			}
		};
		return wxALIGN_INVALID;
	}

	bool SetMasterCSS(const wxString& css)
	{
		auto utf8 = ToSciterUTF8(css);
		return GetSciterAPI()->SciterSetMasterCSS(utf8.data(), utf8.size());
	}
	bool AppendMasterCSS(const wxString& css)
	{
		auto utf8 = ToSciterUTF8(css);
		return GetSciterAPI()->SciterAppendMasterCSS(utf8.data(), utf8.size());
	}
}
