#include "stdafx.h"
#include "IArchive.h"
#include "IArchiveImpl.h"

namespace kxf
{
	bool IArchiveExtract::ExtractToFS(IFileSystem& fileSystem, const FSPath& directory) const
	{
		Compression::FileExtractionCallback callback(const_cast<IArchiveExtract&>(*this), directory);
		return Extract(callback);
	}
	bool IArchiveExtract::ExtractToFS(IFileSystem& fileSystem, const FSPath& directory, Compression::FileIndexView files) const
	{
		Compression::FileExtractionCallback callback(const_cast<IArchiveExtract&>(*this), directory);
		return Extract(callback, files);
	}

	bool IArchiveExtract::ExtractToStream(size_t fileIndex, wxOutputStream& stream) const
	{
		Compression::SingleStreamExtractionCallback callback(const_cast<IArchiveExtract&>(*this), stream);
		return Extract(callback, fileIndex);
	}
}
