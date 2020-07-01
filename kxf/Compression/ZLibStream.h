#pragma once
#include "Common.h"
#include "kxf/IO/StreamWrappers.h"
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
	class ZLibInputStream: public IStreamWrapper, public InputStreamWrapper<wxZlibInputStream>
	{
		public:
			ZLibInputStream(wxInputStream& stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:InputStreamWrapper(stream, ToInt(header))
			{
			}
			ZLibInputStream(wxInputStream* stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:InputStreamWrapper(stream, ToInt(header))
			{
			}

		public:
			bool IsReadable() const override
			{
				return true;
			}
			bool IsWriteable() const override
			{
				return false;
			}
	};

	class ZLibOutputStream: public IStreamWrapper, public OutputStreamWrapper<wxZlibOutputStream>
	{
		public:
			ZLibOutputStream(wxOutputStream& stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:OutputStreamWrapper(stream, ToInt(header))
			{
			}
			ZLibOutputStream(wxOutputStream* stream, ZLibHeaderType header = ZLibHeaderType::Auto)
				:OutputStreamWrapper(stream, ToInt(header))
			{
			}

		public:
			bool IsReadable() const override
			{
				return false;
			}
			bool IsWriteable() const override
			{
				return true;
			}
	};
}
