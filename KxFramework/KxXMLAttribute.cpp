#include "KxStdAfx.h"
#include "KxFramework/KxXML.h"

KxXMLNode KxXMLAttribute::GetNode() const
{
	if (m_Node)
	{
		return *m_Node;
	}
	return {};
}
KxXMLDocument* KxXMLAttribute::GetDocument() const
{
	if (m_Node)
	{
		return &m_Node->GetDocument();
	}
	return nullptr;
}

wxString KxXMLAttribute::GetName() const
{
	if (IsOK())
	{
		return wxString::FromUTF8Unchecked(m_Attribute->Name());
	}
	return wxEmptyString;
}
wxString KxXMLAttribute::GetValue() const
{
	if (IsOK())
	{
		return wxString::FromUTF8Unchecked(m_Attribute->Value());
	}
	return wxEmptyString;
}

KxXMLAttribute KxXMLAttribute::Next() const
{
	if (IsOK())
	{
		if (const tinyxml2::XMLAttribute* attribute = m_Attribute->Next())
		{
			return KxXMLAttribute(*m_Node, *attribute);
		}
	}
	return {};
}
