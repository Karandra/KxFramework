#pragma once
#include "Common.h"
#include "Kx/General/UndefWindows.h"

namespace KxFramework
{
	class FSPath;
	class BinarySize;
	class LegacyDrive;
}

namespace KxFramework
{
	FSPath GetFullPathName(const FSPath& filePath);
	FSPath GetLongPathName(const FSPath& filePath);
	FSPath GetShortPathName(const FSPath& filePath);
	FSPath AbbreviatePath(const FSPath& filePath, int maxCharacters);

	BinarySize GetFileSize(const FSPath& filePath);
	bool DoesExist(const FSPath& filePath);
	bool DoesFileExist(const FSPath& filePath);
	bool DoesDirectoryExist(const FSPath& filePath);
}
