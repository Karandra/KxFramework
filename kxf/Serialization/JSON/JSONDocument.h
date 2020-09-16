#pragma once
#include "../Common.h"
#include "kxf/General/Version.h"
#include "kxf/IO/IStream.h"
#include <nlohmann/json.hpp>

namespace kxf
{
	using JSONDocument = nlohmann::json;
}

namespace kxf::JSON
{
	String GetLibraryName();
	Version GetLibraryVersion();

	JSONDocument Load(const String& json);
	JSONDocument Load(IInputStream& stream);

	String Save(const JSONDocument& json);
	bool Save(const JSONDocument& json, wxOutputStream& stream);
}

namespace nlohmann
{
	template<>
	struct adl_serializer<wxString>
	{
		static void to_json(json& jsonDocument, const wxString& value)
		{
			auto utf8 = value.ToUTF8();
			jsonDocument = std::string_view(utf8.data(), utf8.length());
		}
		static void from_json(const json& jsonDocument, wxString& value)
		{
			if (jsonDocument.is_null())
			{
				value.clear();
			}
			else
			{
				const json::string_t& string = jsonDocument.get_ref<const json::string_t&>();
				value = wxString::FromUTF8Unchecked(string.data(), string.length());
			}
		}
	};

	template<>
	struct adl_serializer<kxf::String>
	{
		static void to_json(json& jsonDocument, const kxf::String& value)
		{
			adl_serializer<wxString>::to_json(jsonDocument, value.GetWxString());
		}
		static void from_json(const json& jsonDocument, kxf::String& value)
		{
			adl_serializer<wxString>::from_json(jsonDocument, value.GetWxString());
		}
	};
}
