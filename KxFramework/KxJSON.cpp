#include "KxStdAfx.h"
#include "KxFramework/KxJSON.h"

wxString KxJSON::GetVersion()
{
	std::string version(16, '\000');
	int length = sprintf_s(version.data(), version.size(), "%d.%d.%d", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
	version.resize(length);

	return version;
}
wxString KxJSON::GetLibraryName()
{
	return "JSON for Modern C++";
}

KxJSONObject KxJSON::Load(const wxString& json)
{
	auto utf8 = json.ToUTF8();
	return KxJSONObject::parse(std::string(utf8.data(), utf8.length()), nullptr, false);
}
KxJSONObject KxJSON::Load(wxInputStream& stream)
{
	wxFileOffset size = stream.GetLength();
	if (size != wxInvalidOffset)
	{
		std::string buffer(size + 1, '\000');
		if(stream.ReadAll(buffer.data(), buffer.size()))
		{
			return KxJSONObject::parse(buffer, nullptr, false);
		}
	}
	return KxJSONObject();
}

wxString KxJSON::Save(const KxJSONObject& json)
{
	std::string string = json.dump(1, '\t');
	return wxString::FromUTF8Unchecked(string.data(), string.length());
}
bool KxJSON::Save(const KxJSONObject& json, wxOutputStream& stream)
{
	std::string string = json.dump(1, '\t');
	return stream.WriteAll(string.data(), string.length());
}
