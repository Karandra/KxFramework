#pragma once
#include "../Common.h"
#include "kxf/General/String.h"
#include "kxf/General/NativeUUID.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/Compression/ArchiveEvent.h"

namespace kxf::SevenZip
{
	enum class CompressionMethod
	{
		Unknown = -1,

		LZMA,
		LZMA2,
		PPMD,
		BZIP2,
	};
	enum class CompressionFormat
	{
		Unknown = -1,

		SevenZip,
		Zip,
		GZip,
		BZip2,
		Rar,
		Rar5,
		Tar,
		Iso,
		Cab,
		Lzma,
		Lzma86
	};
	enum class CompressionLevel
	{
		None = 0,

		Fastest = 1,
		Fast = 2,
		Normal = 5,
		Maximum = 7,
		Ultra = 9,
	};
}

namespace kxf::SevenZip
{
	NativeUUID GetAlgorithmID(CompressionFormat format) noexcept;

	String GetNameByFormat(CompressionFormat format);
	String GetExtensionByFormat(CompressionFormat format);
	CompressionFormat GetFormatByExtension(const String& extension);
}
