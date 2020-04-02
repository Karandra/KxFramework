#include "KxStdAfx.h"
#include "ZLibStream.h"

namespace KxFramework::Compression::ZLib
{
	wxString GetLibraryName()
	{
		return wxGetZlibVersionInfo().GetName();
	}
	KxVersion GetLibraryVersion()
	{
		return wxGetZlibVersionInfo().GetVersionString();
	}
}
