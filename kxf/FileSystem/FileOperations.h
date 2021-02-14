#pragma once
#include "Common.h"
#include "FSPath.h"

namespace kxf::FileSystem
{
	KX_API FSPath CreateTempPathName(const FSPath& rootDirectory);
	KX_API FSPath PathFrom83Name(const FSPath& path);
	KX_API FSPath PathTo83Name(const FSPath& path);
	KX_API FSPath GetAbsolutePath(const FSPath& relativePath);
	KX_API FSPath AbbreviatePath(const FSPath& path, int maxCharacters);
}
