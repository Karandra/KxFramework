#include "KxStdAfx.h"
#include "IArchive.h"
#include "IArchiveImpl.h"
#include <KxFramework/KxFileStream.h>
#include "Kx/FileSystem/NativeFileSystemUtility.h"

namespace KxFramework
{
	FileItem IArchiveItems::FindItem(const FSPathQuery& query) const
	{
		FileItem result;
		EnumItems({}, [&](FileItem item)
		{
			result = std::move(item);
			return false;
		}, query, FSEnumItemsFlag::Recursive);
		return result;
	}
	FileItem IArchiveItems::FindItem(const FSPath& directory, const FSPathQuery& query) const
	{
		FileItem result;
		EnumItems(directory, [&](FileItem item)
		{
			result = std::move(item);
			return false;
		}, query, FSEnumItemsFlag::Recursive);
		return result;
	}
}

namespace KxFramework
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

	bool IArchiveExtraction::ExtractToStream(Compression::FileIndex fileIndex, wxOutputStream& stream) const
	{
		Compression::SingleStreamExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), stream);
		return Extract(callback, fileIndex);
	}
	bool IArchiveExtraction::ExtractToFile(Compression::FileIndex fileIndex, const FSPath& targetPath) const
	{
		Compression::SingleFileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), targetPath);
		return Extract(callback, fileIndex);
	}
}
