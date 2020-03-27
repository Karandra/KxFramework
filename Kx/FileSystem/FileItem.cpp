#include "KxStdAfx.h"
#include "FSPath.h"
#include "FileItem.h"

namespace KxFramework
{
	bool FileItem::DoUpdateInfo()
	{
		// TODO
		return false;
	}

	bool FileItem::IsCurrentOrParent() const
	{
		if (m_Source.GetPathLength() >= 1)
		{
			const wxString name = m_Source.GetName();
			return name == wxS("..") || name == wxS('.');
		}
		return false;
	}
	bool FileItem::IsDirectoryEmpty() const
	{
		return IsDirectory(); // && KxFileFinder::IsDirectoryEmpty(m_Source); TODO
	}
}
