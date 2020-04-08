#include "KxStdAfx.h"
#include "ZLibStream.h"

namespace KxFramework::Compression::ZLib
{
	wxString GetLibraryName()
	{
		return wxGetZlibVersionInfo().GetName();
	}
	Version GetLibraryVersion()
	{
		return wxGetZlibVersionInfo();
	}
}
