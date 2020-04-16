#include "stdafx.h"
#include "ZLibStream.h"

namespace KxFramework::Compression::ZLib
{
	String GetLibraryName()
	{
		return wxGetZlibVersionInfo().GetName();
	}
	Version GetLibraryVersion()
	{
		return wxGetZlibVersionInfo();
	}
}
