#pragma once
#include "Common.h"
#include "String.h"
#include "kxf/FileSystem/FSPath.h"

namespace kxf::TextDocument
{
	enum class LineBreakFormat
	{
		None = -1,

		Windows, // CR LF (13, 10)
		Unix, // LF (10)
		Mac, // CR (13)
	};
}

namespace kxf::TextDocument
{
	size_t Read(const FSPath& filePath, std::function<bool(String)> func);
	String Read(const FSPath& filePath, LineBreakFormat lineBreakFormat = LineBreakFormat::Windows);

	bool Write(const FSPath& filePath, const String& text, bool append = false, LineBreakFormat lineBreakFormat = LineBreakFormat::Windows);
	bool Write(const FSPath& filePath, std::function<String()> func, bool append = false, LineBreakFormat lineBreakFormat = LineBreakFormat::Windows);
}
