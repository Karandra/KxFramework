#include "KxfPCH.h"
#include "INIDocument.h"

namespace
{
	kxf::String ToWxString(const std::string& stdString)
	{
		using namespace kxf;

		return String::FromUTF8(stdString.c_str(), stdString.size());
	}
	kxf::String ToWxString(const char* charString)
	{
		using namespace kxf;

		return String::FromUTF8(charString);
	}
}

namespace kxf
{
	String INIDocument::GetLibraryName()
	{
		return "SimpleINI";
	}
	Version INIDocument::GetLibraryVersion()
	{
		return "4.17";
	}

	void INIDocument::Init()
	{
		m_Document.SetSpaces(false);
		//m_Document.SetAllowEmptyValues(false); // This function has been removed from SimpleINI at some point
	}
	void INIDocument::DoLoad(const char* ini, size_t length)
	{
		m_Document.LoadData(ini, length);
	}
	void INIDocument::DoUnload()
	{
		m_Document.Reset();
	}

	std::optional<String> INIDocument::DoGetValue() const
	{
		return IniGetValue({}, {});
	}
	bool INIDocument::DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA)
	{
		return IniSetValue({}, {}, value, writeEmpty);
	}

	std::optional<String> INIDocument::DoGetAttribute(const String& name) const
	{
		return {};
	}
	bool INIDocument::DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty)
	{
		return false;
	}

	std::optional<String> INIDocument::IniGetValue(const String& sectionName, const String& keyName) const
	{
		auto sectionNameUTF8 = sectionName.ToUTF8();
		auto keyNameUTF8 = keyName.ToUTF8();
		if (const char* value = m_Document.GetValue(sectionNameUTF8.data(), keyNameUTF8.data()))
		{
			return String::FromUTF8(value);
		}
		return {};
	}
	bool INIDocument::IniSetValue(const String& sectionName, const String& keyName, const String& value, WriteEmpty writeEmpty)
	{
		if (writeEmpty == WriteEmpty::Never && value.IsEmpty())
		{
			return false;
		}
		else
		{
			auto sectionNameUTF8 = sectionName.ToUTF8();
			auto keyNameUTF8 = keyName.ToUTF8();
			auto valueUTF8 = value.ToUTF8();

			SimpleINI::SI_Error status = m_Document.SetValue(sectionNameUTF8.data(), keyNameUTF8.data(), valueUTF8.data(), nullptr, true);
			return status == SimpleINI::SI_UPDATED || status == SimpleINI::SI_INSERTED;
		}
	}

	bool INIDocument::Load(const String& ini)
	{
		DoUnload();

		auto utf8 = ini.ToUTF8();
		DoLoad(utf8.data(), utf8.length());
		return !IsNull();
	}
	bool INIDocument::Load(IInputStream& stream)
	{
		DoUnload();

		wxMemoryBuffer buffer(stream.GetSize().ToBytes());
		stream.Read(buffer.GetData(), buffer.GetBufSize());
		buffer.SetDataLen(stream.LastRead().ToBytes());

		DoLoad(reinterpret_cast<const char*>(buffer.GetData()), buffer.GetDataLen());
		return !IsNull();
	}
	bool INIDocument::Save(IOutputStream& stream) const
	{
		std::string buffer;
		m_Document.Save(buffer, false);
		return stream.WriteAll(buffer.data(), buffer.length());
	}
	String INIDocument::Save() const
	{
		std::string buffer;
		m_Document.Save(buffer, false);
		return ToWxString(buffer);
	}
	INIDocument INIDocument::Clone() const
	{
		std::string buffer;
		m_Document.Save(buffer, false);

		return INIDocument(StringViewOf(buffer));
	}

	size_t INIDocument::GetSectionNames(std::function<bool(String)> func) const
	{
		TDocument::TNamesDepend sections;
		m_Document.GetAllSections(sections);
		sections.sort(TDocument::Entry::LoadOrder());

		size_t count = 0;
		for (TDocument::TNamesDepend::const_iterator it = sections.begin(); it != sections.end(); ++it)
		{
			count++;
			if (!std::invoke(func, ToWxString(it->pItem)))
			{
				break;
			}
		}
		return count;
	}
	size_t INIDocument::GetKeyNames(const String& sectionName, std::function<bool(String)> func) const
	{
		auto utf8 = sectionName.ToUTF8();

		TDocument::TNamesDepend keys;
		if (m_Document.GetAllKeys(utf8.data(), keys))
		{
			keys.sort(SimpleINI::CSimpleIniA::Entry::LoadOrder());

			size_t count = 0;
			for (TDocument::TNamesDepend::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				count++;
				if (!std::invoke(func, ToWxString(it->pItem)))
				{
					break;
				}
			}
			return count;
		}
		return 0;
	}

	bool INIDocument::RemoveSection(const String& sectionName)
	{
		auto sectionNameUTF8 = sectionName.ToUTF8();
		return m_Document.DeleteValue(sectionNameUTF8.data(), nullptr, nullptr, true);
	}
	bool INIDocument::RemoveValue(const String& sectionName, const String& keyName)
	{
		auto sectionNameUTF8 = sectionName.ToUTF8();
		auto keyNameUTF8 = keyName.ToUTF8();
		return m_Document.DeleteValue(sectionNameUTF8.data(), keyNameUTF8.data(), nullptr, false);
	}

	bool INIDocument::HasValue(const String& sectionName, const String& keyName)  const
	{
		auto sectionNameUTF8 = sectionName.ToUTF8();
		auto keyNameUTF8 = keyName.ToUTF8();
		return m_Document.GetValue(sectionNameUTF8.data(), keyNameUTF8.data()) != nullptr;
	}
	bool INIDocument::HasSection(const String& sectionName) const
	{
		auto sectionNameUTF8 = sectionName.ToUTF8();
		return m_Document.GetSection(sectionNameUTF8.data()) != nullptr;
	}
}
