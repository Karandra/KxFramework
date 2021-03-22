#include "KxfPCH.h"
#include "Any.h"

namespace
{
	class Formatter2: public kxf::StringFormatter::Formatter<kxf::StringFormatter::DefaultFormatTraits>
	{
		public:
			using Formatter::Formatter;
			using Formatter::FormatIntWithBase;
	};
}

namespace kxf
{
	String Any::IntToString() const noexcept
	{
		if (auto value = AsPtr<int8_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<int16_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<int32_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<int64_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}

		if (auto value = AsPtr<uint8_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<uint16_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<uint32_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<uint64_t>())
		{
			return Formatter2::FormatIntWithBase(*value);
		}
		return {};
	}
	String Any::FloatToString() const noexcept
	{
		if (auto value = AsPtr<float>())
		{
			return Formatter2(wxS("%1"))(*value);
		}
		else if (auto value = AsPtr<double>())
		{
			return Formatter2(wxS("%1"))(*value);
		}
		return {};
	}
	String Any::BoolToString() const noexcept
	{
		if (auto value = AsPtr<bool>())
		{
			return *value ? wxS("true") : wxS("false");
		}
		return {};
	}
	std::optional<bool> Any::StringToBool() const noexcept
	{
		if (const String* ptr = AsPtr<String>())
		{
			if (*ptr == wxS("true") || *ptr == wxS("TRUE"))
			{
				return true;
			}
			else if (*ptr == wxS("false") || *ptr == wxS("FALSE"))
			{
				return false;
			}
		}
		return std::nullopt;
	}
}
