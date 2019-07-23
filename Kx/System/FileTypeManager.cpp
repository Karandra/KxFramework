#include "KxStdAfx.h"
#include "FileTypeManager.h"
#include <KxFramework/KxComparator.h>

bool KxFileTypeManager::IsAssociatedWith(const KxFileType& fileType, const wxString& executablePath) const
{
	if (fileType)
	{
		return KxComparator::IsEqual(fileType.GetOpenExecutable(), executablePath, true);
	}
	return false;
}
