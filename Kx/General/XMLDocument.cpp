#include "stdafx.h"
#include "XMLDocument.h"
#include "StringFormater.h"
#include "Private/XML.h"

namespace KxFramework::XML
{
	String GetLibraryName()
	{
		return wxS("TinyXML2");
	}
	Version GetLibraryVersion()
	{
		return {TIXML2_MAJOR_VERSION, TIXML2_MINOR_VERSION, TIXML2_PATCH_VERSION};
	}
}

namespace KxFramework
{
	void XMLDocument::ReplaceDeclaration()
	{
		if (m_Document.FirstChild() && m_Document.FirstChild()->ToDeclaration())
		{
			m_Document.DeleteNode(m_Document.FirstChild());
		}

		String declaration = String::Format(wxS(R"(xml version="1.0" encoding="%1")"), m_DeclaredEncoding);
		auto utf8 = declaration.ToUTF8();
		m_Document.InsertFirstChild(m_Document.NewDeclaration(utf8.data()));
	}

	void XMLDocument::Init()
	{
		m_DeclaredEncoding = XML::Private::DefaultDeclaredEncoding;
		m_XPathIndexSeparator = XNode::GetXPathIndexSeparator();
		m_Document.SetBOM(false);
	}
	void XMLDocument::DoLoad(const char* xml, size_t length)
	{
		DoUnload();

		m_Document.Parse(xml, length);
		ReplaceDeclaration();
	}
	void XMLDocument::DoUnload()
	{
		m_Document.Clear();
	}

	XMLNode XMLDocument::CreateElement(const String& name)
	{
		auto utf8 = name.ToUTF8();
		return XMLNode(m_Document.NewElement(utf8.data()), *this);
	}
	XMLNode XMLDocument::CreateComment(const String& value)
	{
		auto utf8 = value.ToUTF8();
		return XMLNode(m_Document.NewComment(utf8.data()), *this);
	}
	XMLNode XMLDocument::CreateText(const String& value)
	{
		auto utf8 = value.ToUTF8();
		return XMLNode(m_Document.NewText(utf8.data()), *this);
	}
	XMLNode XMLDocument::CreateDeclaration(const String& value)
	{
		if (!value.IsEmpty())
		{
			auto utf8 = value.ToUTF8();
			return XMLNode(m_Document.NewDeclaration(utf8.data()), *this);
		}
		else
		{
			return XMLNode(m_Document.NewDeclaration(), *this);
		}
	}
	XMLNode XMLDocument::CreateUnknown(const String& value)
	{
		auto utf8 = value.ToUTF8();
		return XMLNode(m_Document.NewUnknown(utf8.data()), *this);
	}

	// General
	String XMLDocument::GetXML(SerializationFormat mode) const
	{
		return XML::Private::PrintDocument(m_Document, mode == SerializationFormat::HTML5);
	}

	bool XMLDocument::Load(const String& xml)
	{
		auto utf8 = xml.ToUTF8();
		DoLoad(utf8.data(), utf8.length());
		return !IsNull();
	}
	bool XMLDocument::Load(std::string_view xml)
	{
		DoLoad(xml.data(), xml.length());
		return !IsNull();
	}
	bool XMLDocument::Load(std::wstring_view xml)
	{
		Load(String::FromView(xml));
		return !IsNull();
	}
	bool XMLDocument::Load(wxInputStream& stream)
	{
		wxMemoryBuffer buffer;
		buffer.SetBufSize(stream.GetLength());
		stream.Read(buffer.GetData(), buffer.GetBufSize());
		buffer.SetDataLen(stream.LastRead());

		DoLoad(reinterpret_cast<const char*>(buffer.GetData()), buffer.GetDataLen());
		return !IsNull();
	}
	bool XMLDocument::Save(wxOutputStream& stream) const
	{
		XML::Private::DefaultXMLPrinter buffer;
		m_Document.Print(&buffer);
		stream.Write(buffer.CStr(), buffer.CStrSize() - 1);
		return stream.IsOk();
	}

	// Deletion
	bool XMLDocument::RemoveNode(XMLNode& node)
	{
		if (!IsNull() && node)
		{
			m_Document.DeleteNode(node.GetNode());
			return true;
		}
		return false;
	}
}
