#include "KxStdAfx.h"
#include "KxFramework/KxXML.h"
#include "KxFramework/KxString.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/XML/TinyXML2 HTML5 Printer.h"

const KxXMLNode KxXMLNode::NullNode = KxXMLNode();

std::string KxXMLNode::CleanText(const tinyxml2::XMLNode* node, const std::string& separator)
{
	std::string contents;
	if (node->ToElement())
	{
		for (const tinyxml2::XMLNode* child = node->FirstChild(); child != nullptr; child = child->NextSibling())
		{
			const std::string text = CleanText(child);
			if (!separator.empty() && child != node && !text.empty())
			{
				contents.append(separator);
			}
			contents.append(text);
		}
	}
	else if (!node->ToDocument())
	{
		contents.assign(node->Value());
	}
	return contents;
}
wxString KxXMLNode::PrintDocument(const tinyxml2::XMLDocument& document, KxXMLPrintMode mode)
{
	if (mode == KxXML_PRINT_HTML5)
	{
		return PrintDocument<tinyxml2::XMLPrinterHTML5>(document);
	}
	else
	{
		return PrintDocument(document);
	}
}
size_t KxXMLNode::GetIndexWithinParent(const tinyxml2::XMLNode* node)
{
	size_t index = 0;
	if (node && node->ToElement())
	{
		auto parentElement = node->Parent() ? node->Parent()->ToElement() : nullptr;
		if (parentElement)
		{
			for (const tinyxml2::XMLElement* element = parentElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
			{
				index++;
				if (element == node->ToElement())
				{
					break;
				}
			}
		}
	}
	return index;
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
bool KxXMLNode::DoSetValue(const wxString& value, AsCDATA asCDATA)
{
	auto node = GetNode();
	if (node)
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
bool KxXMLNode::DoSetAttribute(const wxString& name, const wxString& value)
{
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

KxXMLNode::KxXMLNode(tinyxml2::XMLNode* node, KxXMLDocument* document)
	:m_Node(node), m_Document(document)
{
}
KxXMLNode::KxXMLNode(const tinyxml2::XMLNode* node, KxXMLDocument* document)
	: m_Node(const_cast<tinyxml2::XMLNode*>(node)), m_Document(document)
{
}

KxXMLNode KxXMLNode::QueryElement(const wxString& XPath) const
{
	if (!IsOK())
	{
		return {};
	}

	KxStringVector elements = KxString::Split(XPath, wxS("/"));
	if (!elements.empty())
	{
		const tinyxml2::XMLNode* currentNode = GetNode();
		const tinyxml2::XMLNode* previousNode = currentNode;

		for (wxString& name: elements)
		{
			if (!currentNode)
			{
				break;
			}

			// Save previous element
			previousNode = currentNode;

			// Extract index from name and remove it from path
			// point/x -> 1, point/x::2 -> 2, point/y::0 -> 1, point/z::-7 -> 1
			int index = m_Document->ExtractIndexFromName(name);
			auto elementNameUTF8 = name.ToUTF8();

			// Get level 1
			currentNode = previousNode->FirstChildElement(elementNameUTF8.data());

			// We need to go down by 'index' elements
			if (index != 1 && currentNode)
			{
				for (int level = 1; level < index && currentNode; level++)
				{
					// Get next level
					currentNode = currentNode->NextSiblingElement(elementNameUTF8.data());
				}
			}
		}
		return KxXMLNode(const_cast<tinyxml2::XMLNode*>(currentNode), const_cast<KxXMLDocument*>(GetDocumentNode()));
	}
	return NullNode;
}
KxXMLNode KxXMLNode::QueryOrCreateElement(const wxString& XPath)
{
	if (!IsOK())
	{
		return {};
	}

	KxStringVector elements = KxString::Split(XPath, wxS("/"));
	if (!elements.empty())
	{
		KxXMLNode currentNode = *this;
		KxXMLNode previousNode = currentNode;

		for (wxString& name: elements)
		{
			// Save previous element
			previousNode = currentNode;

			// Extract index from name and remove it from path
			// point/x -> 1, point/x::2 -> 2, point/y::0 -> 1, point/z::-7 -> 1
			const int index = m_Document->ExtractIndexFromName(name);

			// Get level 1
			currentNode = previousNode.GetFirstChildElement(name);
			if (!currentNode.IsOK())
			{
				currentNode = previousNode.NewElement(name, KxXML_INSERT_AS_LAST_CHILD);
			}

			// We need to get or create 'index' amount of elements
			if (index > 1)
			{
				for (int level = 1; level < index; level++)
				{
					KxXMLNode tempNode = currentNode.GetNextSiblingElement(name);
					if (tempNode.IsOK())
					{
						currentNode = tempNode;
					}
					else
					{
						currentNode = currentNode.NewElement(name, KxXML_INSERT_AFTER_CHILD);
					}
				}
			}
		}
		return currentNode;
	}
	return NullNode;
}

wxString KxXMLNode::GetXPathIndexSeparator() const
{
	return m_Document ? m_Document->GetXPathIndexSeparator() : wxEmptyString;
}
bool KxXMLNode::SetXPathIndexSeparator(const wxString& value)
{
	return m_Document ? m_Document->SetXPathIndexSeparator(value) : false;
}

size_t KxXMLNode::GetIndexWithinParent() const
{
	return GetIndexWithinParent(GetNode());
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
	NodeVector list;
	if (auto node = GetNode())
	{
		for (const tinyxml2::XMLNode* child = node->FirstChild(); child != nullptr; child = child->NextSibling())
		{
			list.push_back(KxXMLNode(const_cast<tinyxml2::XMLNode*>(child), m_Document));
		}
	}
	return list;
}
KxXMLNode::NodeVector KxXMLNode::GetChildrenElements(const wxString& searchPattern) const
{
	NodeVector list;
	if (auto node = GetNode())
	{
		for (const tinyxml2::XMLElement* child = node->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			if (searchPattern.IsEmpty() || wxString(child->Name()).Matches(searchPattern))
			{
				list.push_back(KxXMLNode(const_cast<tinyxml2::XMLElement*>(child), m_Document));
			}
		}
	}
	return list;
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
		return PrintDocument(subTree, mode);
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
			return KxXML_NODE_DOCUMENT;
		}
		else if (node->ToElement())
		{
			return KxXML_NODE_ELEMENT;
		}
		else if (node->ToText())
		{
			return KxXML_NODE_TEXT;
		}
		else if (node->ToComment())
		{
			return KxXML_NODE_COMMENT;
		}
		else if (node->ToDeclaration())
		{
			return KxXML_NODE_DECLARATION;
		}
		else if (node->ToUnknown())
		{
			return KxXML_NODE_UNKNOWN;
		}
	}
	return KxXML_NODE_INVALID;
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
	if (GetNode())
	{
		if (auto node = GetNode())
		{
			auto utf8 = separator.ToUTF8();
			return ToWxString(CleanText(node, utf8.data()));
		}
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
			for (const tinyxml2::XMLAttribute* attribute = node->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
			{
				list.push_back(attribute->Name());
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

/* Navigation */
KxXMLNode KxXMLNode::GetElementByAttribute(const wxString& name, const wxString& value) const
{
	if (auto node = GetNode())
	{
		if (GetAttribute(name) == value)
		{
			return KxXMLNode(GetNode(), m_Document);
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
	return NullNode;
}
KxXMLNode KxXMLNode::GetElementByTag(const wxString& tagName) const
{
	auto node = GetNode();
	if (node && node->ToElement())
	{
		if (GetName() == tagName)
		{
			return KxXMLNode(GetNode(), m_Document);
		}
		else
		{
			for (auto child = GetFirstChild(); child.IsOK(); child = child.GetNextSibling())
			{
				if (child.IsElement())
				{
					auto foundElement = child.GetElementByTag(tagName);
					if (foundElement.IsOK())
					{
						return foundElement;
					}
				}
			}
		}
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetParent() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->Parent(), m_Document);
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetPreviousSibling() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->PreviousSibling(), m_Document);
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetPreviousSiblingElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->PreviousSiblingElement(), m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->PreviousSiblingElement(utf8.data()), m_Document);
		}
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetNextSibling() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->NextSibling(), m_Document);
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetNextSiblingElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->NextSiblingElement(), m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->NextSiblingElement(utf8.data()), m_Document);
		}
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetFirstChild() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->FirstChild(), m_Document);
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetFirstChildElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->FirstChildElement(), m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->FirstChildElement(utf8.data()), m_Document);
		}
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetLastChild() const
{
	if (auto node = GetNode())
	{
		return KxXMLNode(node->LastChild(), m_Document);
	}
	return NullNode;
}
KxXMLNode KxXMLNode::GetLastChildElement(const wxString& name) const
{
	if (auto node = GetNode())
	{
		if (name.IsEmpty())
		{
			return KxXMLNode(node->LastChildElement(), m_Document);
		}
		else
		{
			auto utf8 = name.ToUTF8();
			return KxXMLNode(node->LastChildElement(utf8.data()), m_Document);
		}
	}
	return NullNode;
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
bool KxXMLNode::Insert(KxXMLNode& node, KxXMLInsertNodeMode insertMode)
{
	switch (insertMode)
	{
		case KxXML_INSERT_AFTER_CHILD:
		{
			return InsertAfterChild(node);
		}
		case KxXML_INSERT_AS_FIRST_CHILD:
		{
			return InsertFirstChild(node);
		}
		case KxXML_INSERT_AS_LAST_CHILD:
		{
			return InsertLastChild(node);
		}
	};
	return false;
}

/* Insertion */
KxXMLNode KxXMLNode::NewElement(const wxString& name, KxXMLInsertNodeMode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateElement(name);
		return Insert(node, insertMode) ? node : NullNode;
	}
	return {};
}
KxXMLNode KxXMLNode::NewComment(const wxString& value, KxXMLInsertNodeMode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateComment(value);
		return Insert(node, insertMode) ? node : NullNode;
	}
	return {};
}
KxXMLNode KxXMLNode::NewText(const wxString& value, KxXMLInsertNodeMode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateText(value);
		return Insert(node, insertMode) ? node : NullNode;
	}
	return {};
}
KxXMLNode KxXMLNode::NewDeclaration(const wxString& value, KxXMLInsertNodeMode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateDeclaration(value);
		return Insert(node, insertMode) ? node : NullNode;
	}
	return {};
}
KxXMLNode KxXMLNode::NewUnknown(const wxString& value, KxXMLInsertNodeMode insertMode)
{
	if (m_Document)
	{
		KxXMLNode node = m_Document->CreateUnknown(value);
		return Insert(node, insertMode) ? node : NullNode;
	}
	return {};
}
