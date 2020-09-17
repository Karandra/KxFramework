#include "stdafx.h"
#include "JSONDocument.h"

namespace kxf::JSON
{
	String GetLibraryName()
	{
		return wxS("JSON for Modern C++");
	}
	Version GetLibraryVersion()
	{
		return {NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH};
	}

	JSONDocument Load(const String& json)
	{
		auto utf8 = json.ToUTF8();
		return JSONDocument::parse(std::string(utf8.data(), utf8.length()), nullptr, false);
	}
	JSONDocument Load(IInputStream& stream)
	{
		if (auto size = stream.GetSize())
		{
			std::string buffer(size.GetBytes() + 1, '\000');
			if (stream.ReadAll(buffer.data(), buffer.size()))
			{
				return JSONDocument::parse(buffer, nullptr, false);
			}
		}
		return {};
	}

	String Save(const JSONDocument& json)
	{
		std::string string = json.dump(1, '\t');
		return String::FromUTF8(string.data(), string.length());
	}
	bool Save(const JSONDocument& json, IOutputStream& stream)
	{
		std::string string = json.dump(1, '\t');
		return stream.WriteAll(string.data(), string.length());
	}
}
