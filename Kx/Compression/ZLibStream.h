#pragma once
#include "Common.h"
#include "Kx/General/StreamWrappers.h"
#include "Kx/General/Version.h"
#include "Kx/General/String.h"
#include <wx/zstream.h>

namespace KxFramework
{
	enum class ZLibHeaderType
	{
		None = wxZLIB_NO_HEADER,
		Auto = wxZLIB_AUTO,
		ZLib = wxZLIB_ZLIB,
		GZip = wxZLIB_GZIP,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowCast(ZLibHeaderType);
	}
}

namespace KxFramework::Compression::ZLib
{
	String GetLibraryName();
	Version GetLibraryVersion();
}

namespace KxFramework
{
	class ZLibInputStream: public IStreamWrapper, public InputStreamWrapper<wxZlibInputStream>
	{
		public:
			ZLibInputStream(wxInputStream& stream, ZLibHeaderType flags = ZLibHeaderType::Auto)
				:InputStreamWrapper(stream, ToInt(flags))
			{
			}
			ZLibInputStream(wxInputStream* stream, ZLibHeaderType flags = ZLibHeaderType::Auto)
				:InputStreamWrapper(stream, ToInt(flags))
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
			ZLibOutputStream(wxOutputStream& stream, ZLibHeaderType flags = ZLibHeaderType::Auto)
				:OutputStreamWrapper(stream, ToInt(flags))
			{
			}
			ZLibOutputStream(wxOutputStream* stream, ZLibHeaderType flags = ZLibHeaderType::Auto)
				:OutputStreamWrapper(stream, ToInt(flags))
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
