#pragma once
#include "Common.h"
#include "Kx/General/UndefWindows.h"

namespace KxFramework
{
	class FSPath;
	class BinarySize;
	class LegacyDrive;
}

namespace KxFramework::FileSystem
{
	FSPath CreateTempPathName(const FSPath& rootDirectory);
	FSPath GetFullPathName(const FSPath& path);
	FSPath GetLongPathName(const FSPath& path);
	FSPath GetShortPathName(const FSPath& path);
	FSPath AbbreviatePath(const FSPath& path, int maxCharacters);
}
