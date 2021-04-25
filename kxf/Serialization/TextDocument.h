#pragma once
#include "Common.h"
#include "String.h"
#include "kxf/FileSystem/FSPath.h"

namespace kxf::TextDocument
{
	size_t Read(const FSPath& filePath, std::function<bool(String)> func);
	String Read(const FSPath& filePath, LineBreakFormat lineBreakFormat = LineBreakFormat::Windows);

	bool Write(const FSPath& filePath, const String& text, bool append = false, LineBreakFormat lineBreakFormat = LineBreakFormat::Windows);
	bool Write(const FSPath& filePath, std::function<String()> func, bool append = false, LineBreakFormat lineBreakFormat = LineBreakFormat::Windows);
}
