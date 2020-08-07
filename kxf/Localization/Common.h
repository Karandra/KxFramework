#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/ResourceID.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/FileSystem/IFileSystem.h"
#include "Constants.h"

namespace kxf
{
	class Locale;
	class DynamicLibrary;
}

namespace kxf::Localization
{
	String GetStandardString(int id);
	String GetStandardString(StdID id);

	size_t SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, std::function<bool(Locale, FileItem)> func);
	size_t SearchPackages(const DynamicLibrary& library, std::function<bool(Locale, FileItem)> func);
}
