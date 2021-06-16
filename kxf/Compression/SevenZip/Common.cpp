#include "KxfPCH.h"
#include "Common.h"
#include "Library.h"
#include "kxf/System/ExecutableVersionResource.h"
#include "Private/GUIDs.h"

namespace kxf::SevenZip
{
	String GetLibraryName()
	{
		return wxS("7-Zip");
	}
	Version GetLibraryVersion()
	{
		Library& library = Library::GetInstance();
		if (library.IsLoaded())
		{
			ExecutableVersionResource versionResource(library.GetLibrary().GetFilePath());
			if (versionResource)
			{
				return versionResource.GetAnyVersion();
			}
		}
		return {};
	}

	NativeUUID GetAlgorithmID(CompressionFormat format) noexcept
	{
		switch (format)
		{
			case CompressionFormat::SevenZip:
			{
				return GUID::CLSID_CFormat7z;
			}
			case CompressionFormat::Zip:
			{
				return GUID::CLSID_CFormatZip;
			}
			case CompressionFormat::GZip:
			{
				return GUID::CLSID_CFormatGZip;
			}
			case CompressionFormat::BZip2:
			{
				return GUID::CLSID_CFormatBZip2;
			}
			case CompressionFormat::Rar:
			{
				return GUID::CLSID_CFormatRar;
			}
			case CompressionFormat::Rar5:
			{
				return GUID::CLSID_CFormatRar5;
			}
			case CompressionFormat::Tar:
			{
				return GUID::CLSID_CFormatTar;
			}
			case CompressionFormat::Iso:
			{
				return GUID::CLSID_CFormatIso;
			}
			case CompressionFormat::Cab:
			{
				return GUID::CLSID_CFormatCab;
			}
			case CompressionFormat::Lzma:
			{
				return GUID::CLSID_CFormatLzma;
			}
			case CompressionFormat::Lzma86:
			{
				return GUID::CLSID_CFormatLzma86;
			}
		}
		return {};
	}
	String GetNameByFormat(CompressionFormat format)
	{
		switch (format)
		{
			case CompressionFormat::SevenZip:
			{
				return wxS("7-Zip");
			}
			case CompressionFormat::Zip:
			{
				return wxS("ZIP");
			}
			case CompressionFormat::Rar:
			{
				return wxS("RAR");
			}
			case CompressionFormat::Rar5:
			{
				return wxS("RAR5");
			}
			case CompressionFormat::GZip:
			{
				return wxS("GZip");
			}
			case CompressionFormat::BZip2:
			{
				return wxS("BZip2");
			}
			case CompressionFormat::Tar:
			{
				return wxS("TAR");
			}
			case CompressionFormat::Lzma:
			{
				return wxS("LZMA");
			}
			case CompressionFormat::Lzma86:
			{
				return wxS("LZMA 86");
			}
			case CompressionFormat::Cab:
			{
				return wxS("CAB");
			}
			case CompressionFormat::Iso:
			{
				return wxS("ISO");
			}
		}
		return {};
	}
	String GetExtensionByFormat(CompressionFormat format)
	{
		switch (format)
		{
			case CompressionFormat::SevenZip:
			{
				return wxS("7z");
			}
			case CompressionFormat::Zip:
			{
				return wxS("zip");
			}
			case CompressionFormat::Rar:
			case CompressionFormat::Rar5:
			{
				return wxS("rar");
			}
			case CompressionFormat::GZip:
			{
				return wxS("gz");
			}
			case CompressionFormat::BZip2:
			{
				return wxS("bz2");
			}
			case CompressionFormat::Tar:
			{
				return wxS("tar");
			}
			case CompressionFormat::Lzma:
			{
				return wxS("lzma");
			}
			case CompressionFormat::Lzma86:
			{
				return wxS("lzma86");
			}
			case CompressionFormat::Cab:
			{
				return wxS("cab");
			}
			case CompressionFormat::Iso:
			{
				return wxS("iso");
			}
		}
		return {};
	}
	CompressionFormat GetFormatByExtension(const String& extension)
	{
		auto Test = [&](const XChar* ext)
		{
			return extension.IsSameAs(ext, StringActionFlag::IgnoreCase);
		};

		if (Test(wxS("7z")))
		{
			return CompressionFormat::SevenZip;
		}
		if (Test(wxS("zip")))
		{
			return CompressionFormat::Zip;
		}
		if (Test(wxS("rar")))
		{
			return CompressionFormat::Rar;
		}
		if (Test(wxS("rar5")))
		{
			return CompressionFormat::Rar5;
		}
		if (Test(wxS("gz")) || Test(wxS("gzip")))
		{
			return CompressionFormat::GZip;
		}
		if (Test(wxS("bz2")) || Test(wxS("bzip2")))
		{
			return CompressionFormat::BZip2;
		}
		if (Test(wxS("tar")))
		{
			return CompressionFormat::Tar;
		}
		if (Test(wxS("lz")) || Test(wxS("lzma")))
		{
			return CompressionFormat::Lzma;
		}
		if (Test(wxS("lz86")) || Test(wxS("lzma86")))
		{
			return CompressionFormat::Lzma86;
		}
		if (Test(wxS("cab")))
		{
			return CompressionFormat::Cab;
		}
		if (Test(wxS("iso")))
		{
			return CompressionFormat::Iso;
		}

		return CompressionFormat::Unknown;
	}
}
