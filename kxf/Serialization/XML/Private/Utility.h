#pragma once
#include "kxf/Serialization/Common.h"
#include "TinyXML2 HTML5 Printer.h"

namespace kxf::XML::Private
{
	constexpr wxChar DefaultDeclaredEncoding[] = wxS("utf-8");

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

namespace kxf::XML::Private
{
	inline String ToString(const char* value, size_t length = wxString::npos)
	{
		return String::FromUTF8(value, length);
	}
	inline String CleanText(const tinyxml2::XMLNode& node, StringView separator = {})
	{
		String content;
		if (node.ToElement())
		{
			for (const tinyxml2::XMLNode* child = node.FirstChild(); child; child = child->NextSibling())
			{
				String text = CleanText(*child);
				if (!separator.empty() && child != &node && !text.empty())
				{
					content += separator;
				}
				content += text;
			}
		}
		else if (!node.ToDocument())
		{
			content = ToString(node.Value());
		}
		return content;
	}

	template<class T = DefaultXMLPrinter>
	String PrintDocumentUsing(const tinyxml2::XMLDocument& document)
	{
		T buffer;
		document.Print(&buffer);
		return ToString(buffer.CStr(), buffer.CStrSize() - 1);
	}

	inline String PrintDocument(const tinyxml2::XMLDocument& document, bool asHTML5)
	{
		if (asHTML5)
		{
			return PrintDocumentUsing<tinyxml2::XMLPrinterHTML5>(document);
		}
		return PrintDocumentUsing(document);
	}
}
