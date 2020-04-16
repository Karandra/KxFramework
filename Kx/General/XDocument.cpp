#include "stdafx.h"
#include "XDocument.h"
#include "Kx/Utility/String.h"
#include <charconv>

namespace
{
	template<class TBuffer, class TValue>
	bool IntToChars(TBuffer&& buffer, TValue value, int base = 10)
	{
		return std::to_chars(std::begin(buffer), std::end(buffer), value, base).ec == std::errc();
	}

	template<class TBuffer, class TValue>
	bool FloatToChars(TBuffer&& buffer, TValue value, int precision)
	{
		return std::to_chars(std::begin(buffer), std::end(buffer), value, std::chars_format::fixed, precision).ec == std::errc();
	}
}

namespace KxFramework::XDocument
{
	std::pair<StringView, int> IXNode::ExtractIndexFromName(StringView elementName, StringView xPathSeparator)
	{
		int index = 0;
		if (!xPathSeparator.empty())
		{
			size_t indexStart = elementName.find(xPathSeparator);
			if (indexStart != String::npos)
			{
				if (auto value = String::FromView(elementName.substr(indexStart + xPathSeparator.length())).ToInt<int>(10))
				{
					index = std::clamp(*value, 0, std::numeric_limits<int>::max());
					elementName = elementName.substr(0, indexStart);
				}
			}
		}
		return {elementName, index};
	}
	bool IXNode::ContainsValueForbiddenCharacters(const String& value)
	{
		for (wxChar c: value)
		{
			if (c == wxS('&') || c == wxS('<') || c == wxS('>'))
			{
				return true;
			}
		}
		return false;
	}

	String IXNode::FormatInt(int64_t value, int base) const
	{
		char buffer[64] = {0};
		if (IntToChars(buffer, value, base))
		{
			return buffer;
		}
		return {};
	}
	String IXNode::FormatPointer(const void* value) const
	{
		char buffer[64] = {};
		if (IntToChars(buffer, reinterpret_cast<size_t>(value), 16))
		{
			return String(wxS("0x")) + buffer;
		}
		return {};
	}
	String IXNode::FormatFloat(double value, int precision) const
	{
		char buffer[128] = {0};
		if (FloatToChars(buffer, value, precision))
		{
			return buffer;
		}
		return {};
	}
	String IXNode::FormatBool(bool value) const
	{
		return value ? wxS("1") : wxS("0");
	}

	std::optional<int64_t> IXNode::ParseInt(const String& value, int base) const
	{
		return value.ToInt<int64_t>(base);
	}
	std::optional<void*> IXNode::ParsePointer(const String& value) const
	{
		String intValue;
		if (value.StartsWith(wxS("0x"), &intValue))
		{
			if (auto iValue = intValue.ToInt<size_t>(16))
			{
				return reinterpret_cast<void*>(*iValue);
			}
		}
		return {};
	}
	std::optional<double> IXNode::ParseFloat(const String& value) const
	{
		return value.ToFloatingPoint<double>();
	}
	std::optional<bool> IXNode::ParseBool(const String& value) const
	{
		return Utility::ParsePool(value);
	}
}
