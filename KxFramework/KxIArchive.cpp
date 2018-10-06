#include "KxStdAfx.h"
#include "KxFramework/KxIArchive.h"
#include "KxFramework/KxFileItem.h"
#include "KxFramework/KxArchiveFileFinder.h"
#include "KxFramework/KxComparator.h"

float KxIArchive::GetCompressionRatio() const
{
	int64_t originalSize = GetOriginalSize();
	int64_t compressedSize = GetCompressedSize();

	if (originalSize > 0)
	{
		return (double)compressedSize / originalSize;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool KxIArchiveSearch::FindFile(const wxString& searchQuery, KxFileItem& fileItem) const
{
	KxArchiveFileFinder finder(*this, searchQuery);
	fileItem = finder.FindNext();
	return fileItem.IsOK();
}
bool KxIArchiveSearch::FindFileInFolder(const wxString& folder, const wxString& filter, KxFileItem& fileItem) const
{
	KxArchiveFileFinder finder(*this, folder, filter);

	KxFileItem item = finder.FindNext();
	while (item.IsOK())
	{
		if (KxComparator::IsEqual(folder, item.GetSource(), true))
		{
			fileItem = item;
			return true;
		}
		item = finder.FindNext();
	}
	return false;
}
