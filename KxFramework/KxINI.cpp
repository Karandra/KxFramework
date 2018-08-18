#include "KxStdAfx.h"
#include "KxFramework/KxINI.h"

bool KxINI::Load(const char* iniText, size_t length)
{
	m_Document.SetSpaces(false);
	return m_Document.LoadData(iniText, length) == SimpleINI::SI_OK;
}
void KxINI::UnLoad()
{
	m_Document.Reset();
}

wxString KxINI::DoGetValue(const wxString& sectionName, const wxString& keyName, const wxString& defaultValue) const
{
	auto sectionNameUTF8 = sectionName.ToUTF8();
	auto keyNameUTF8 = keyName.ToUTF8();
	const char* value = m_Document.GetValue(sectionNameUTF8.data(), keyNameUTF8.data());

	return value ? wxString::FromUTF8Unchecked(value) : defaultValue;
}
bool KxINI::DoSetValue(const wxString& sectionName, const wxString& keyName, const wxString& value)
{
	auto sectionNameUTF8 = sectionName.ToUTF8();
	auto keyNameUTF8 = keyName.ToUTF8();
	auto valueUTF8 = value.ToUTF8();

	SimpleINI::SI_Error status = m_Document.SetValue(sectionNameUTF8.data(), keyNameUTF8.data(), valueUTF8.data(), NULL, true);
	return status == SimpleINI::SI_UPDATED || status == SimpleINI::SI_INSERTED;
}

KxINI::KxINI(const wxString& iniText)
	:m_Document(true, false, false, true)
{
	if (!iniText.IsEmpty())
	{
		Load(iniText);
	}
}
KxINI::KxINI(wxInputStream& stream)
	:m_Document(true, false, false, true)
{
	Load(stream);
}
KxINI::~KxINI()
{
	UnLoad();
}

bool KxINI::IsOK() const
{
	return !m_Document.IsEmpty();
}

bool KxINI::Load(const wxString& xmlText)
{
	UnLoad();

	auto utf8 = xmlText.ToUTF8();
	Load(utf8.data(), utf8.length());
	return IsOK();
}
bool KxINI::Load(wxInputStream& stream)
{
	UnLoad();

	wxMemoryBuffer buffer(stream.GetLength());
	stream.Read(buffer.GetData(), buffer.GetBufSize());
	buffer.SetDataLen(stream.LastRead());

	Load((const char*)buffer.GetData(), buffer.GetDataLen());
	return IsOK();
}
bool KxINI::Save(wxOutputStream& stream) const
{
	std::string buffer;
	m_Document.Save(buffer, false);
	stream.Write(buffer.data(), buffer.length());
	return stream.IsOk();
}
wxString KxINI::Save() const
{
	std::string buffer;
	m_Document.Save(buffer, false);
	return ToWxString(buffer);
}

KxStringVector KxINI::GetSectionNames() const
{
	DocumentT::TNamesDepend sections;
	m_Document.GetAllSections(sections);
	sections.sort(DocumentT::Entry::LoadOrder());

	KxStringVector list;
	list.reserve(sections.size());
	for (DocumentT::TNamesDepend::const_iterator it = sections.begin(); it != sections.end(); ++it)
	{
		list.push_back(ToWxString(it->item));
	}
	return list;
}
KxStringVector KxINI::GetKeyNames(const wxString& sectionName) const
{
	KxStringVector list;
	DocumentT::TNamesDepend keys;

	auto utf8 = sectionName.ToUTF8();
	if (m_Document.GetAllKeys(utf8.data(), keys))
	{
		keys.sort(SimpleINI::CSimpleIniA::Entry::LoadOrder());
		list.reserve(keys.size());

		for (DocumentT::TNamesDepend::const_iterator it = keys.begin(); it != keys.end(); ++it)
		{
			list.push_back(ToWxString(it->item));
		}
	}
	return list;
}

bool KxINI::RemoveSection(const wxString& sectionName)
{
	auto sectionNameUTF8 = sectionName.ToUTF8();
	return m_Document.DeleteValue(sectionNameUTF8.data(), NULL, NULL, true);
}
bool KxINI::RemoveValue(const wxString& sectionName, const wxString& keyName)
{
	auto sectionNameUTF8 = sectionName.ToUTF8();
	auto keyNameUTF8 = keyName.ToUTF8();
	return m_Document.DeleteValue(sectionNameUTF8.data(), keyNameUTF8.data(), NULL, false);
}

bool KxINI::HasValue(const wxString& sectionName, const wxString& keyName)  const
{
	auto sectionNameUTF8 = sectionName.ToUTF8();
	auto keyNameUTF8 = keyName.ToUTF8();
	return m_Document.GetValue(sectionNameUTF8.data(), keyNameUTF8.data()) != NULL;
}
bool KxINI::HasSection(const wxString& sectionName) const
{
	auto sectionNameUTF8 = sectionName.ToUTF8();
	return m_Document.GetSection(sectionNameUTF8.data()) != NULL;
}
