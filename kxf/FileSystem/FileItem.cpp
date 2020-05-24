#include "stdafx.h"
#include "FSPath.h"
#include "FileItem.h"
#include "IFileSystem.h"

namespace kxf
{
	FileItem& FileItem::Refresh(const IFileSystem& fileSystem)
	{
		*this = fileSystem.GetItem(m_Path);
		return *this;
	}
}
