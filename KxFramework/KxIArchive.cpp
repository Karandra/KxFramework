#include "KxStdAfx.h"
#include "KxFramework/KxIArchive.h"
#include "KxFramework/KxFileItem.h"
#include "KxFramework/KxArchiveFileFinder.h"
#include "KxFramework/KxComparator.h"

namespace KxArchive
{
	double IArchive::GetCompressionRatio() const
	{
		int64_t originalSize = GetOriginalSize();
		int64_t compressedSize = GetCompressedSize();

		if (originalSize > 0)
		{
			return (double)compressedSize / originalSize;
		}
		return 0;
	}
}

namespace KxArchive
{
	bool IArchiveSearch::FindFile(const wxString& searchQuery, KxFileItem& fileItem) const
	{
		FileFinder finder(*this, searchQuery);
		fileItem = finder.FindNext();

		return fileItem.IsOK();
	}
	bool IArchiveSearch::FindFileInFolder(const wxString& folder, const wxString& filter, KxFileItem& fileItem) const
	{
		FileFinder finder(*this, folder, filter);

		for (KxFileItem item = finder.FindNext(); item.IsOK(); item = finder.FindNext())
		{
			if (KxComparator::IsEqual(folder, item.GetSource(), true))
			{
				fileItem = std::move(item);
				return true;
			}
		}
		return false;
	}
}
