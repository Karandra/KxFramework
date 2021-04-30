#include "KxfPCH.h"
#include "JSONDocument.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/Utility/SoftwareLicenseDB.h"

namespace
{
	constexpr kxf::XChar g_Copyright[] = wxS("Copyright© 2013-2021 Niels Lohmann");
}

namespace kxf
{
	String JSONDocument::Save() const
	{
		return String::FromUTF8(this->dump(1, '\t'));
	}
	bool JSONDocument::Save(IOutputStream& stream) const
	{
		std::string string = this->dump(1, '\t');
		return stream.WriteAll(string.data(), string.length());
	}

	bool JSONDocument::Load(const String& json)
	{
		*this = JSONDocument::parse(json.ToUTF8(), nullptr, false);
		return this->empty();
	}
	bool JSONDocument::Load(IInputStream& stream)
	{
		if (auto size = stream.GetSize())
		{
			IO::InputStreamReader reader(stream);

			*this = JSONDocument::parse(reader.ReadStdString(size.ToBytes()), nullptr, false);
			return this->empty();
		}
		else
		{
			this->clear();
			return false;
		}
	}

	// ILibraryInfo
	String JSONDocument::GetName() const
	{
		return wxS("JSON for Modern C++");
	}
	Version JSONDocument::GetVersion() const
	{
		return {NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH};
	}
	uint32_t JSONDocument::GetAPILevel() const
	{
		return NLOHMANN_JSON_VERSION_MAJOR * 1000 + NLOHMANN_JSON_VERSION_MINOR * 100 + NLOHMANN_JSON_VERSION_PATCH * 10;
	}

	String JSONDocument::GetLicense() const
	{
		return SoftwareLicenseDB::Get().GetText(SoftwareLicenseType::MIT, g_Copyright);
	}
	String JSONDocument::GetLicenseName() const
	{
		return SoftwareLicenseDB::Get().GetName(SoftwareLicenseType::MIT);
	}
	String JSONDocument::GetCopyright() const
	{
		return g_Copyright;
	}
}
