#pragma once
#include "Common.h"
#include "FSPath.h"

namespace KxFramework::FileSystem
{
	FSPath CreateTempPathName(const FSPath& rootDirectory);
	FSPath GetFullPathName(const FSPath& path);
	FSPath GetLongPathName(const FSPath& path);
	FSPath GetShortPathName(const FSPath& path);
	FSPath AbbreviatePath(const FSPath& path, int maxCharacters);
}
