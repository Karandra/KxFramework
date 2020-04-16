#include "stdafx.h"
#include "String.h"

namespace KxFramework::Utility
{
	std::optional<bool> ParsePool(const String& value)
	{
		if (value.IsSameAs(wxS("false"), StringOpFlag::IgnoreCase))
		{
			return false;
		}
		else if (value.IsSameAs(wxS("true"), StringOpFlag::IgnoreCase))
		{
			return true;
		}
		else if (auto iValue = value.ToInt<int>())
		{
			return *iValue != 0;
		}
		else if (auto fValue = value.ToFloatingPoint())
		{
			return *fValue != 0;
		}
		return {};
	}
}
