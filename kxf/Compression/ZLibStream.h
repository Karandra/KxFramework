#pragma once
#include "Common.h"
#include "kxf/General/Version.h"
#include "kxf/General/String.h"
#include <wx/zstream.h>

namespace kxf
{
	enum class ZLibHeaderType
	{
		None = wxZLIB_NO_HEADER,
		Auto = wxZLIB_AUTO,
		ZLib = wxZLIB_ZLIB,
		GZip = wxZLIB_GZIP,
	};
}

namespace kxf::Compression::ZLib
{
	String GetLibraryName();
	Version GetLibraryVersion();
}

namespace kxf
{
	class ZLibInputStream: public wxZlibInputStream
	{
		public:
			ZLibInputStream(wxInputStream& stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibInputStream(stream, ToInt(header))
			{
			}
			ZLibInputStream(wxInputStream* stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibInputStream(stream, ToInt(header))
			{
			}
	};

	class ZLibOutputStream: public wxZlibOutputStream
	{
		public:
			ZLibOutputStream(wxOutputStream& stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibOutputStream(stream, ToInt(header))
			{
			}
			ZLibOutputStream(wxOutputStream* stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:wxZlibOutputStream(stream, ToInt(header))
			{
			}
	};
}
