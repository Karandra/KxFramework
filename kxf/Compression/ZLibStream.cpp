#include "KxfPCH.h"
#include "ZLibStream.h"

namespace kxf::Compression::ZLib
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
