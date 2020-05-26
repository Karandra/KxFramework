#pragma once
#include "Common.h"
#include "FSPath.h"

namespace kxf::FileSystem
{
	FSPath CreateTempPathName(const FSPath& rootDirectory);
	FSPath PathFrom83Name(const FSPath& path);
	FSPath PathTo83Name(const FSPath& path);
	FSPath GetAbsolutePath(const FSPath& relativePath);
	FSPath AbbreviatePath(const FSPath& path, int maxCharacters);
}
