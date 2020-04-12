#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/XML/TinyXML2 HTML5 Printer.h"

namespace
{
	constexpr wxChar g_DefaultDeclaredEncoding[] = wxS("utf-8");

	class DefaultXMLPrinter: public tinyxml2::XMLPrinter
	{
		public:
			DefaultXMLPrinter(FILE* file = nullptr, bool compact = false, int depth = 0)
				:XMLPrinter(file, compact, depth)
			{
			}

		protected:
			void PrintSpace(int depth) override
			{
				for (int i = 0; i < depth; i++)
				{
					Write("\t");
				}
			}
	};
}

namespace
{
	template<class T = DefaultXMLPrinter>
	wxString PrintDocument(const tinyxml2::XMLDocument& document)
	{
		T buffer;
		document.Print(&buffer);
		return wxString::FromUTF8Unchecked(buffer.CStr(), buffer.CStrSize() - 1);
	}

	inline wxString PrintDocument(const tinyxml2::XMLDocument& document, bool asHTML5)
	{
		if (asHTML5)
		{
			return PrintDocument<tinyxml2::XMLPrinterHTML5>(document);
		}
		return PrintDocument(document);
	}

	inline wxString ToWxString(const char* value, size_t length = wxString::npos)
	{
		return wxString::FromUTF8Unchecked(value, length);
	}
	inline wxString CleanText(const tinyxml2::XMLNode& node, wxStringView separator = {})
	{
		wxString content;
		if (node.ToElement())
		{
			for (const tinyxml2::XMLNode* child = node.FirstChild(); child; child = child->NextSibling())
			{
				wxString text = CleanText(*child);
				if (!separator.empty() && child != &node && !text.empty())
				{
					content.append(separator.data(), separator.size());
				}
				content += text;
			}
		}
		else if (!node.ToDocument())
		{
			content = ToWxString(node.Value());
		}
		return content;
	}
}
