#include "KxStdAfx.h"
#include "KxFramework/KxXML.h"
#include "KxFramework/KxXMLUtility.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxString.h"
#include "KxFramework/KxStringUtility.h"
#include "KxFramework/XML/TinyXML2 HTML5 Printer.h"

KxXMLNode KxXMLNode::ConstructOrQueryElement(const wxString& xPath, bool allowCreate)
{
	if (IsOK())
	{
		tinyxml2::XMLDocument& document = GetDocument().m_Document;
		tinyxml2::XMLNode* currentNode = GetNode();
		tinyxml2::XMLNode* previousNode = currentNode;

		size_t itemCount = KxUtility::String::SplitBySeparator(xPath, GetXPathSeparator(), [&](wxStringView name)
		{
			// Save previous element
			if (!currentNode)
			{
				return false;
			}
			previousNode = currentNode;

			// Extract index from name and remove it from path, zero-based
			// point/x -> 0, point/x::1 -> 1, point/y::0 -> 0, point/z::-7 -> 0
			auto [elementName, index] = KxXDocumentNode::ExtractIndexFromName(name, KxUtility::String::ToStringView(GetDocument().m_XPathIndexSeparator));
			auto elementNameUTF8 = KxUtility::String::FromStringView(name).ToUTF8();

			// Get level 0
			currentNode = previousNode->FirstChildElement(elementNameUTF8.data());
			if (!currentNode)
			{
				if (allowCreate)
				{
					currentNode = document.NewElement(elementNameUTF8.data());
					previousNode->InsertEndChild(currentNode);
				}
				else
				{
					return false;
				}
			}

			// We need to go down by 'index' more elements
			for (int level = 1; level <= index; level++)
			{
				// Get next level
				currentNode = currentNode->NextSiblingElement(elementNameUTF8.data());
				if (!currentNode)
				{
					if (allowCreate)
					{
						currentNode = document.NewElement(elementNameUTF8.data());
						previousNode->InsertAfterChild(previousNode, currentNode);
					}
					else
					{
						return false;
					}
				}
			}
			return true;
		});

		if (currentNode && itemCount != 0)
		{
			return KxXMLNode(currentNode, GetDocument());
		}
	}
	return {};
}

wxString KxXMLNode::DoGetValue(const wxString& defaultValue) const
{
	if (auto node = GetNode())
	{
		if (auto text = node->ToText())
		{
			return ToWxString(text->Value());
		}
		else if (auto element = node->ToElement())
		{
			if (const char* text = element->GetText())
			{
				return ToWxString(text);
			}
		}
		else if (const char* value = node->Value())
		{
			return ToWxString(value);
		}
	}
	return defaultValue;
}
int64_t KxXMLNode::DoGetValueIntWithBase(int base, int64_t defaultValue) const
{
	if (base == 10)
	{
		int64_t value = defaultValue;
		if (auto node = GetNode())
		{
			if (auto element = node->ToElement())
			{
				element->QueryInt64Text(&value);
			}
		}
		return value;
	}
	return KxXDocumentNode::DoGetValueIntWithBase(base, defaultValue);
}
double KxXMLNode::DoGetValueFloat(double defaultValue) const
{
	double value = defaultValue;
	if (auto node = GetNode())
	{
		if (auto element = node->ToElement())
		{
			element->QueryDoubleText(&value);
		}
	}
	return value;
}
bool KxXMLNode::DoGetValueBool(bool defaultValue) const
{
	bool value = defaultValue;
	if (auto node = GetNode())
	{
		if (auto element = node->ToElement())
		{
			element->QueryBoolText(&value);
		}
	}
	return value;
}
bool KxXMLNode::DoSetValue(const wxString& value, WriteEmpty writeEmpty, AsCDATA asCDATA)
{
	if (writeEmpty == WriteEmpty::Never && value.IsEmpty())
	{
		return false;
	}

	if (auto node = GetNode())
	{
		if (node->ToElement())
		{
			if (!value.IsEmpty())
			{
				auto utf8 = value.ToUTF8();
				tinyxml2::XMLText* textNode = m_Document->GetDocument()->NewText(utf8.data());

				switch (asCDATA)
				{
					case AsCDATA::Always:
					{
						textNode->SetCData(true);
						break;
					}
					case AsCDATA::Never:
					{
						textNode->SetCData(false);
						break;
					}
					default:
					{
						textNode->SetCData(ContainsForbiddenCharactersForValue(value));
						break;
					}
				};

				node->DeleteChildren();
				node->InsertFirstChild(textNode);

				return true;
			}
		}
		else if (node->ToText() || node->ToComment() || node->ToDeclaration() || node->ToUnknown())
		{
			auto utf8 = value.ToUTF8();
			node->SetValue(utf8.data());
			return true;
		}
	}
	return false;
}

wxString KxXMLNode::DoGetAttribute(const wxString& name, const wxString& defaultValue) const
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			auto utf8 = name.ToUTF8();
			const char* value = node->Attribute(utf8.data());
			if (value)
			{
				return ToWxString(value);
			}
		}
	}
	return defaultValue;
}
int64_t KxXMLNode::DoGetAttributeIntWithBase(const wxString& name, int base, int64_t defaultValue) const
{
	if (base == 10)
	{
		int64_t value = defaultValue;
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto utf8 = name.ToUTF8();
				node->QueryInt64Attribute(utf8.data(), &value);
			}
		}
		return value;
	}
	return KxXDocumentNode::DoGetAttributeIntWithBase(name, base, defaultValue);
}
double KxXMLNode::DoGetAttributeFloat(const wxString& name, double defaultValue) const
{
	double value = defaultValue;
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			auto utf8 = name.ToUTF8();
			node->QueryDoubleAttribute(utf8.data(), &value);
		}
	}
	return value;
}
bool KxXMLNode::DoGetAttributeBool(const wxString& name, bool defaultValue) const
{
	bool value = defaultValue;
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			auto utf8 = name.ToUTF8();
			node->QueryBoolAttribute(utf8.data(), &value);
		}
	}
	return value;
}
bool KxXMLNode::DoSetAttribute(const wxString& name, const wxString& value, WriteEmpty writeEmpty)
{
	if (writeEmpty == WriteEmpty::Never && value.IsEmpty())
	{
		return false;
	}

	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			auto nameUTF8 = name.ToUTF8();
			auto valueUTF8 = value.ToUTF8();
			node->SetAttribute(nameUTF8.data(), valueUTF8.data());
			return true;
		}
	}
	return false;
}

KxXMLNode KxXMLNode::QueryElement(const wxString& xPath) const
{
	return const_cast<KxXMLNode&>(*this).ConstructOrQueryElement(xPath, false);
}
KxXMLNode KxXMLNode::ConstructElement(const wxString& xPath)
{
	return const_cast<KxXMLNode&>(*this).ConstructOrQueryElement(xPath, true);
}

wxString KxXMLNode::GetXPathIndexSeparator() const
{
	return m_Document ? m_Document->GetXPathIndexSeparator() : wxEmptyString;
}
void KxXMLNode::SetXPathIndexSeparator(const wxString& value)
{
	if (m_Document)
	{
		m_Document->SetXPathIndexSeparator(value);
	}
}

size_t KxXMLNode::GetIndexWithinParent() const
{
	if (auto node = GetNode(); node && node->ToElement())
	{
		if (auto parentElement = node->Parent() ? node->Parent()->ToElement() : nullptr)
		{
			size_t index = 0;
			for (const tinyxml2::XMLElement* element = parentElement->FirstChildElement(); element; element = element->NextSiblingElement())
			{
				index++;
				if (element == node->ToElement())
				{
					break;
				}
			}
			return index;
		}
	}
	return 0;
}
wxString KxXMLNode::GetName() const
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			return ToWxString(node->Name());
		}
	}
	return wxEmptyString;
}
bool KxXMLNode::SetName(const wxString& name)
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			auto utf8 = name.ToUTF8();
			node->SetName(utf8.data());
			return true;
		}
	}
	return false;
}

size_t KxXMLNode::GetChildrenCount() const
{
	size_t count = 0;
	if (auto node = GetNode())
	{
		for (const tinyxml2::XMLNode* child = node->FirstChild(); child != nullptr; child = child->NextSibling())
		{
			count++;
		}
	}
	return count;
}
bool KxXMLNode::HasChildren() const
{
	auto node = GetNode();
	if (node)
	{
		return !node->NoChildren();
	}
	return false;
}
KxXMLNode::NodeVector KxXMLNode::GetChildren() const
{
	NodeVector items;
	if (auto node = GetNode())
	{
		for (const tinyxml2::XMLNode* child = node->FirstChild(); child; child = child->NextSibling())
		{
			items.emplace_back(KxXMLNode(child, *m_Document));
		}
	}
	return items;
}
KxXMLNode::NodeVector KxXMLNode::GetChildrenElements(const wxString& searchPattern) const
{
	NodeVector items;
	if (auto node = GetNode())
	{
		for (const tinyxml2::XMLElement* child = node->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			if (searchPattern.IsEmpty() || wxString::FromUTF8Unchecked(child->Name()).Matches(searchPattern))
			{
				items.emplace_back(KxXMLNode(child, *m_Document));
			}
		}
	}
	return items;
}
bool KxXMLNode::ClearChildren()
{
	if (auto node = GetNode())
	{
		node->DeleteChildren();
		return true;
	}
	return false;
}
bool KxXMLNode::ClearNode()
{
	ClearChildren();
	ClearAttributes();
	return true;
}

wxString KxXMLNode::GetXML(KxXMLPrintMode mode) const
{
	auto node = GetNode();
	if (node)
	{
		tinyxml2::XMLDocument subTree;
		node->DeepClone(&subTree);
		return PrintDocument(subTree, mode == KxXMLPrintMode::HTML5);
	}
	return wxEmptyString;
}
KxXMLNodeType KxXMLNode::GetType() const
{
	auto node = GetNode();
	if (node)
	{
		if (node->ToDocument())
		{
			return KxXMLNodeType::Document;
		}
		else if (node->ToElement())
		{
			return KxXMLNodeType::Element;
		}
		else if (node->ToText())
		{
			return KxXMLNodeType::Text;
		}
		else if (node->ToComment())
		{
			return KxXMLNodeType::Comment;
		}
		else if (node->ToDeclaration())
		{
			return KxXMLNodeType::Declaration;
		}
		else if (node->ToUnknown())
		{
			return KxXMLNodeType::Unknown;
		}
	}
	return KxXMLNodeType::Invalid;
}
bool KxXMLNode::IsElement() const
{
	if (GetNode())
	{
		return GetNode()->ToElement() != nullptr;
	}
	return false;
}
bool KxXMLNode::IsText() const
{
	if (GetNode())
	{
		return GetNode()->ToText() != nullptr;
	}
	return false;
}

wxString KxXMLNode::GetValueText(const wxString& separator) const
{
	if (auto node = GetNode())
	{
		return CleanText(*node, KxUtility::String::ToStringView(separator));
	}
	return wxEmptyString;
}
bool KxXMLNode::IsCDATA() const
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToText())
		{
			return node->CData();
		}
	}
	return false;
}
bool KxXMLNode::SetCDATA(bool value)
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToText())
		{
			node->SetCData(value);
			return true;
		}
	}
	return false;
}

size_t KxXMLNode::GetAttributeCount() const
{
	size_t count = 0;
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			for (const tinyxml2::XMLAttribute* attribute = node->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
			{
				count++;
			}
		}
	}
	return count;
}
bool KxXMLNode::HasAttributes() const
{
	if (auto node = GetNode())
	{
		if (auto element = node->ToElement())
		{
			return element->FirstAttribute() != nullptr;
		}
	}
	return false;
}
KxStringVector KxXMLNode::GetAttributes() const
{
	KxStringVector list;
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			for (const tinyxml2::XMLAttribute* attribute = node->FirstAttribute(); attribute; attribute = attribute->Next())
			{
				list.emplace_back(ToWxString(attribute->Name()));
			}
		}
	}
	return list;
}

bool KxXMLNode::HasAttribute(const wxString& name) const
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			auto utf8 = name.ToUTF8();
			return node->Attribute(utf8.data()) != nullptr;
		}
	}
	return false;
}
bool KxXMLNode::RemoveAttribute(const wxString& name)
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			auto tName = name.ToUTF8();
			node->DeleteAttribute(tName.data());
			return true;
		}
	}
	return false;
}
bool KxXMLNode::RemoveAttribute(KxXMLAttribute& attribute)
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			node->DeleteAttribute(attribute.GetAttribute()->Name());
			return true;
		}
	}
	return false;
}
bool KxXMLNode::ClearAttributes()
{
	if (GetNode())
	{
		if (auto node = GetNode()->ToElement())
		{
			for (const tinyxml2::XMLAttribute* attribute = node->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
			{
				node->DeleteAttribute(attribute->Name());
			}
			return true;
		}
	}
	return false;
}

// Navigation
KxXMLNode KxXMLNode::GetElementByAttribute(const wxString& name, const wxString& value) const
{
	if (auto node = GetNode())
	{
		if (GetAttribute(name) == value)
		{
			return KxXMLNode(node, *m_Document);
		}
		else
		{
			for (auto child = GetFirstChild(); child.IsOK(); child = child.GetNextSibling())
			{
				auto foundElement = child.GetElementByAttribute(name, value);
				if (foundElement.IsOK())
				{
					return foundElement;
				}
			}
		}
	}
	return {};
}
KxXMLNode KxXMLNode::GetElementByTag(const wxString& tagName) const
{
	auto node = GetNode();
	if (node && node->ToElement())
	{
		if (GetName() == tagName)
		{
			return KxXMLNode(node, *m_Document);
		}
		else
		{
			for (auto child = GetFirstChild(); child; child = child.GetNextSibling())
			{
				if (child.IsElement())
				{
					if (KxXMLNode foundElement = child.GetElementByTag(tagName))
					{
						return foundElement;
					}
				}
			}
		}
	}
	return {};
}
KxXMLNode KxXMLNode::GetParent() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->Parent(), *m_Document);
	}
	return {};
}
KxXMLNode KxXMLNode::GetPreviousSibling() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->PreviousSibling(), *m_Document);
	}
	return {};
}
KxXMLNode KxXMLNode::GetPreviousSiblingElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->PreviousSiblingElement(), *m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->PreviousSiblingElement(utf8.data()), *m_Document);
		}
	}
	return {};
}
KxXMLNode KxXMLNode::GetNextSibling() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->NextSibling(), *m_Document);
	}
	return {};
}
KxXMLNode KxXMLNode::GetNextSiblingElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->NextSiblingElement(), *m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->NextSiblingElement(utf8.data()), *m_Document);
		}
	}
	return {};
}
KxXMLNode KxXMLNode::GetFirstChild() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->FirstChild(), *m_Document);
	}
	return {};
}
KxXMLNode KxXMLNode::GetFirstChildElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->FirstChildElement(), *m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->FirstChildElement(utf8.data()), *m_Document);
		}
	}
	return {};
}
KxXMLNode KxXMLNode::GetLastChild() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->LastChild(), *m_Document);
	}
	return {};
}
KxXMLNode KxXMLNode::GetLastChildElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->LastChildElement(), *m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->LastChildElement(utf8.data()), *m_Document);
		}
	}
	return {};
}

bool KxXMLNode::InsertAfterChild(KxXMLNode& newNode)
{
	auto thisTxNode = GetNode();
	auto newTxNode = newNode.GetNode();

	if (thisTxNode && newTxNode)
	{
		return thisTxNode->InsertAfterChild(thisTxNode, newTxNode) != nullptr;
	}
	return false;
}
bool KxXMLNode::InsertFirstChild(KxXMLNode& newNode)
{
	auto thisTxNode = GetNode();
	auto newTxNode = newNode.GetNode();

	if (thisTxNode && newTxNode)
	{
		return thisTxNode->InsertFirstChild(newTxNode) != nullptr;
	}
	return false;
}
bool KxXMLNode::InsertLastChild(KxXMLNode& newNode)
{
	auto thisTxNode = GetNode();
	auto newTxNode = newNode.GetNode();

	if (thisTxNode && newTxNode)
	{
		return thisTxNode->InsertEndChild(newTxNode) != nullptr;
	}
	return false;
}
bool KxXMLNode::Insert(KxXMLNode& node, KxXMLInsertNode insertMode)
{
	switch (insertMode)
	{
		case KxXMLInsertNode::AfterChild:
		{
			return InsertAfterChild(node);
		}
		case KxXMLInsertNode::AsFirstChild:
		{
			return InsertFirstChild(node);
		}
		case KxXMLInsertNode::AsLastChild:
		{
			return InsertLastChild(node);
		}
	};
	return false;
}

// Insertion
KxXMLNode KxXMLNode::NewElement(const wxString& name, KxXMLInsertNode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateElement(name);
		if (node && Insert(node, insertMode))
		{
			return node;
		}
	}
	return {};
}
KxXMLNode KxXMLNode::NewComment(const wxString& value, KxXMLInsertNode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateComment(value);
		if (node && Insert(node, insertMode))
		{
			return node;
		}
	}
	return {};
}
KxXMLNode KxXMLNode::NewText(const wxString& value, KxXMLInsertNode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateText(value);
		if (node && Insert(node, insertMode))
		{
			return node;
		}
	}
	return {};
}
KxXMLNode KxXMLNode::NewDeclaration(const wxString& value, KxXMLInsertNode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateDeclaration(value);
		if (node && Insert(node, insertMode))
		{
			return node;
		}
	}
	return {};
}
KxXMLNode KxXMLNode::NewUnknown(const wxString& value, KxXMLInsertNode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateUnknown(value);
		if (node && Insert(node, insertMode))
		{
			return node;
		}
	}
	return {};
}
