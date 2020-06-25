#include "stdafx.h"
#include "IArchive.h"
#include "IArchiveImpl.h"

namespace kxf
{
	bool IArchiveExtraction::ExtractToDirectory(const FSPath& directory) const
	{
		Compression::FileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), directory);
		return Extract(callback);
	}
	bool IArchiveExtraction::ExtractToDirectory(const FSPath& directory, Compression::FileIndexView files) const
	{
		Compression::FileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), directory);
		return Extract(callback, files);
	}

	bool IArchiveExtraction::ExtractToStream(size_t fileIndex, wxOutputStream& stream) const
	{
		Compression::SingleStreamExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), stream);
		return Extract(callback, fileIndex);
	}
	bool IArchiveExtraction::ExtractToFile(size_t fileIndex, const FSPath& targetPath) const
	{
		Compression::SingleFileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), targetPath);
		return Extract(callback, fileIndex);
	}
}
