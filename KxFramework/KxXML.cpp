#include "KxStdAfx.h"
#include "KxFramework/KxXML.h"
#include "KxFramework/KxXMLUtility.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxStringUtility.h"

wxString KxXMLDocument::GetLibraryName()
{
	return wxS("TinyXML2");
}
wxString KxXMLDocument::GetLibraryVersion()
{
	return KxString::Format(wxS("%1.%2.%3"), TIXML2_MAJOR_VERSION, TIXML2_MINOR_VERSION, TIXML2_PATCH_VERSION);
}

void KxXMLDocument::ReplaceDeclaration()
{
	if (m_Document.FirstChild() && m_Document.FirstChild()->ToDeclaration())
	{
		m_Document.DeleteNode(m_Document.FirstChild());
	}

	wxString declaration = wxS("xml version=\"1.0\" encoding=\"" + m_DeclaredEncoding + "\"");
	auto utf8 = declaration.ToUTF8();
	m_Document.InsertFirstChild(m_Document.NewDeclaration(utf8.data()));
}

void KxXMLDocument::Init()
{
	m_DeclaredEncoding = g_DefaultDeclaredEncoding;
	m_XPathIndexSeparator = KxXDocumentNode::GetXPathIndexSeparator();
	m_Document.SetBOM(false);
}
bool KxXMLDocument::DoLoad(const char* xml, size_t length)
{
	DoUnload();

	m_Document.Parse(xml, length);
	ReplaceDeclaration();
	return !m_Document.Error();
}
void KxXMLDocument::DoUnload()
{
	m_Document.Clear();
}

KxXMLNode KxXMLDocument::CreateElement(const wxString& name)
{
	auto utf8 = name.ToUTF8();
	return KxXMLNode(m_Document.NewElement(utf8.data()), *this);
}
KxXMLNode KxXMLDocument::CreateComment(const wxString& value)
{
	auto utf8 = value.ToUTF8();
	return KxXMLNode(m_Document.NewComment(utf8.data()), *this);
}
KxXMLNode KxXMLDocument::CreateText(const wxString& value)
{
	auto utf8 = value.ToUTF8();
	return KxXMLNode(m_Document.NewText(utf8.data()), *this);
}
KxXMLNode KxXMLDocument::CreateDeclaration(const wxString& value)
{
	if (!value.IsEmpty())
	{
		auto utf8 = value.ToUTF8();
		return KxXMLNode(m_Document.NewDeclaration(utf8.data()), *this);
	}
	else
	{
		return KxXMLNode(m_Document.NewDeclaration(), *this);
	}
}
KxXMLNode KxXMLDocument::CreateUnknown(const wxString& value)
{
	auto utf8 = value.ToUTF8();
	return KxXMLNode(m_Document.NewUnknown(utf8.data()), *this);
}

bool KxXMLDocument::IsOK() const
{
	return !m_Document.Error() && m_Document.FirstChild() != nullptr;
}
wxString KxXMLDocument::GetXPath() const
{
	return wxEmptyString;
}
wxString KxXMLDocument::GetXML(KxXMLPrintMode mode) const
{
	return PrintDocument(m_Document, mode == KxXMLPrintMode::HTML5);
}

bool KxXMLDocument::Load(const wxString& xml)
{
	auto utf8 = xml.ToUTF8();
	DoLoad(utf8.data(), utf8.length());
	return IsOK();
}
bool KxXMLDocument::Load(std::string_view xml)
{
	DoLoad(xml.data(), xml.length());
	return IsOK();
}
bool KxXMLDocument::Load(std::wstring_view xml)
{
	return Load(KxUtility::String::FromStringView(xml));
}
bool KxXMLDocument::Load(wxInputStream& stream)
{
	wxMemoryBuffer buffer;
	buffer.SetBufSize(stream.GetLength());
	stream.Read(buffer.GetData(), buffer.GetBufSize());
	buffer.SetDataLen(stream.LastRead());

	DoLoad(reinterpret_cast<const char*>(buffer.GetData()), buffer.GetDataLen());
	return IsOK();
}
bool KxXMLDocument::Save(wxOutputStream& stream) const
{
	DefaultXMLPrinter buffer;
	m_Document.Print(&buffer);
	stream.Write(buffer.CStr(), buffer.CStrSize() - 1);
	return stream.IsOk();
}
wxString KxXMLDocument::Save() const
{
	return GetXML();
}

/* Deletion */
bool KxXMLDocument::RemoveNode(KxXMLNode& node)
{
	if (IsOK() && node.IsOK())
	{
		m_Document.DeleteNode(node.GetNode());
		return true;
	}
	return false;
}

KxXMLDocument& KxXMLDocument::operator=(const KxXMLDocument& other)
{
	m_DeclaredEncoding = other.m_DeclaredEncoding;
	m_XPathIndexSeparator = other.m_XPathIndexSeparator;

	if (!other)
	{
		DoUnload();
	}
	else
	{
		// Serialize and parse into a new document
		Load(other.GetXML());
	}
	return *this;
}
