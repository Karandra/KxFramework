#pragma once
#include "Common.h"
#include "String.h"
#include "Version.h"
#include <nlohmann/json.hpp>

namespace KxFramework
{
	using JSONDocument = nlohmann::json;
}

namespace KxFramework::JSON
{
	String GetLibraryName();
	Version GetLibraryVersion();

	JSONDocument Load(const String& json);
	JSONDocument Load(wxInputStream& stream);

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
	struct adl_serializer<KxFramework::String>
	{
		static void to_json(json& jsonDocument, const KxFramework::String& value)
		{
			adl_serializer<wxString>::to_json(jsonDocument, value.GetWxString());
		}
		static void from_json(const json& jsonDocument, KxFramework::String& value)
		{
			adl_serializer<wxString>::from_json(jsonDocument, value.GetWxString());
		}
	};
}
