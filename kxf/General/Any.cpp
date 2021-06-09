#include "KxfPCH.h"
#include "Any.h"
#include "kxf/General/String.h"

namespace kxf
{
	String Any::IntToString() const noexcept
	{
		if (auto value = AsPtr<int8_t>())
		{
			return ToString(*value);
		}
		else if (auto value = AsPtr<int16_t>())
		{
			return ToString(*value);
		}
		else if (auto value = AsPtr<int32_t>())
		{
			return ToString(*value);
		}
		else if (auto value = AsPtr<int64_t>())
		{
			return ToString(*value);
		}

		if (auto value = AsPtr<uint8_t>())
		{
			return ToString(*value);
		}
		else if (auto value = AsPtr<uint16_t>())
		{
			return ToString(*value);
		}
		else if (auto value = AsPtr<uint32_t>())
		{
			return ToString(*value);
		}
		else if (auto value = AsPtr<uint64_t>())
		{
			return ToString(*value);
		}
		return {};
	}
	String Any::FloatToString() const noexcept
	{
		if (auto value = AsPtr<float>())
		{
			return ToString(*value);
		}
		else if (auto value = AsPtr<double>())
		{
			return ToString(*value);
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
