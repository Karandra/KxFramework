#include "KxStdAfx.h"
#include "KxFramework/KxXML.h"

const KxXMLAttribute KxXMLAttribute::NullAttribute = KxXMLAttribute();

KxXMLAttribute::KxXMLAttribute(const tinyxml2::XMLAttribute* attribute, KxXMLNode* node)
	:m_Node(node), m_Attribute(const_cast<tinyxml2::XMLAttribute*>(attribute))
{
}
KxXMLAttribute::KxXMLAttribute(tinyxml2::XMLAttribute* attribute, KxXMLNode* node)
	: m_Node(node), m_Attribute(attribute)
{
}
KxXMLAttribute::~KxXMLAttribute()
{
}

wxString KxXMLAttribute::GetName() const
{
	if (IsOK())
	{
		return KxXMLNode::ToWxString(m_Attribute->Name());
	}
	return wxEmptyString;
}
wxString KxXMLAttribute::GetValue() const
{
	if (IsOK())
	{
		return KxXMLNode::ToWxString(m_Attribute->Value());
	}
	return wxEmptyString;
}

KxXMLAttribute KxXMLAttribute::Next() const
{
	if (IsOK())
	{
		return KxXMLAttribute(m_Attribute->Next(), m_Node);
	}
	return NullAttribute;
}
