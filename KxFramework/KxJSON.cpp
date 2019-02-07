#include "KxStdAfx.h"
#include "KxFramework/KxJSON.h"

wxString KxJSON::GetLibraryName()
{
	return wxS("JSON for Modern C++");
}
wxString KxJSON::GetLibraryVersion()
{
	return KxString::Format("%1.%2.%3", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
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
