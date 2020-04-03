#include "KxStdAfx.h"
#include "Any.h"
#include "KxFormat.h"

namespace
{
	class Formatter: public KxFormat<KxFormatTraits>
	{
		public:
			using KxFormat::KxFormat;
			using KxFormat::FormatIntWithBase;
	};
}

namespace KxFramework
{
	wxString Any::IntToString() const noexcept
	{
		if (auto value = AsPtr<int8_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<int16_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<int32_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<int64_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}

		if (auto value = AsPtr<uint8_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<uint16_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<uint32_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}
		else if (auto value = AsPtr<uint64_t>())
		{
			return Formatter::FormatIntWithBase(*value);
		}
		return {};
	}
	wxString Any::FloatToString() const noexcept
	{
		if (auto value = AsPtr<float>())
		{
			return Formatter(wxS("%1"))(*value);
		}
		else if (auto value = AsPtr<double>())
		{
			return Formatter(wxS("%1"))(*value);
		}
		return {};
	}
	wxString Any::BoolToString() const noexcept
	{
		if (auto value = AsPtr<bool>())
		{
			return *value ? wxS("true") : wxS("false");
		}
		return {};
	}
	std::optional<bool> Any::StringToBool() const noexcept
	{
		if (const wxString* ptr = AsPtr<wxString>())
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
