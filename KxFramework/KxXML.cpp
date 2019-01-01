#include "KxStdAfx.h"
#include "KxFramework/KxXML.h"
#include "KxFramework/KxUtility.h"

namespace
{
	const wxChar DefaultDeclaredEncoding[] = wxS("utf-8");
}

wxString KxXMLDocument::GetLibraryName()
{
	return wxS("TinyXML2");
}
wxString KxXMLDocument::GetLibraryVersion()
{
	return KxString::Format(wxS("%1.%2.%3"), TIXML2_MAJOR_VERSION, TIXML2_MINOR_VERSION, TIXML2_PATCH_VERSION);
}

int KxXMLDocument::ExtractIndexFromName(wxString& elementName) const
{
	return KxXDocumentNode::ExtractIndexFromName(elementName, m_XPathDelimiter);
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
	m_DeclaredEncoding = DefaultDeclaredEncoding;
	m_XPathDelimiter = KxXDocumentNode::GetXPathIndexSeparator();
	m_Document.SetBOM(false);
}
bool KxXMLDocument::Load(const char* xmlText, size_t length)
{
	m_Document.Parse(xmlText, length);
	ReplaceDeclaration();
	return !m_Document.Error();
}
void KxXMLDocument::UnLoad()
{
	m_Document.Clear();
}

KxXMLNode KxXMLDocument::CreateElement(const wxString& name)
{
	auto utf8 = name.ToUTF8();
	return KxXMLNode(m_Document.NewElement(utf8.data()), this);
}
KxXMLNode KxXMLDocument::CreateComment(const wxString& value)
{
	auto utf8 = value.ToUTF8();
	return KxXMLNode(m_Document.NewComment(utf8.data()), this);
}
KxXMLNode KxXMLDocument::CreateText(const wxString& value)
{
	auto utf8 = value.ToUTF8();
	return KxXMLNode(m_Document.NewText(utf8.data()), this);
}
KxXMLNode KxXMLDocument::CreateDeclaration(const wxString& value)
{
	if (!value.IsEmpty())
	{
		auto utf8 = value.ToUTF8();
		return KxXMLNode(m_Document.NewDeclaration(utf8.data()), this);
	}
	else
	{
		return KxXMLNode(m_Document.NewDeclaration(), this);
	}
}
KxXMLNode KxXMLDocument::CreateUnknown(const wxString& value)
{
	auto utf8 = value.ToUTF8();
	return KxXMLNode(m_Document.NewUnknown(utf8.data()), this);
}

KxXMLDocument::KxXMLDocument(const wxString& xmlText)
	:KxXMLNode(&m_Document, this)
{
	Init();
	Load(xmlText);
}
KxXMLDocument::KxXMLDocument(wxInputStream& stream)
	:KxXMLNode(&m_Document, this)
{
	Init();
	Load(stream);
}
KxXMLDocument::~KxXMLDocument()
{
	UnLoad();
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
	return PrintDocument(m_Document, mode);
}

bool KxXMLDocument::Load(const wxString& xmlText)
{
	UnLoad();

	auto utf8 = xmlText.ToUTF8();
	Load(utf8.data(), utf8.length());
	return IsOK();
}
bool KxXMLDocument::Load(wxInputStream& stream)
{
	UnLoad();

	wxMemoryBuffer buffer;
	buffer.SetBufSize(stream.GetLength());
	stream.Read(buffer.GetData(), buffer.GetBufSize());
	buffer.SetDataLen(stream.LastRead());

	Load((const char*)buffer.GetData(), buffer.GetDataLen());
	return IsOK();
}
bool KxXMLDocument::Save(wxOutputStream& stream) const
{
	KxXMLPrinter buffer;
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
