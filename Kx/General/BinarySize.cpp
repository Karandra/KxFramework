#include "KxStdAfx.h"
#include "BinarySize.h"
#include "StringFormater.h"
#include <KxFramework/KxTranslation.h>
#include <KxFramework/KxMath.h>

namespace KxFramework
{
	String GetSizeUnitString(BinarySizeUnit unit)
	{
		switch (unit)
		{
			case BinarySizeUnit::Bytes:
			{
				return KxTranslation::GetCurrent().GetString(wxS("BinarySizeUnit.Bytes"));
			}
			case BinarySizeUnit::KiloBytes:
			{
				return KxTranslation::GetCurrent().GetString(wxS("BinarySizeUnit.KB"));
			}
			case BinarySizeUnit::MegaBytes:
			{
				return KxTranslation::GetCurrent().GetString(wxS("BinarySizeUnit.MB"));
			}
			case BinarySizeUnit::GigaBytes:
			{
				return KxTranslation::GetCurrent().GetString(wxS("BinarySizeUnit.GB"));
			}
			case BinarySizeUnit::TeraBytes:
			{
				return KxTranslation::GetCurrent().GetString(wxS("BinarySizeUnit.TB"));
			}
		};
		return {};
	}
	bool AddUnitLabelIfNeeded(String& result, BinarySizeFormat format, BinarySizeUnit unit)
	{
		if (format & BinarySizeFormat::WithLabel && !result.IsEmpty())
		{
			String label = GetSizeUnitString(unit);
			if (!label.IsEmpty())
			{
				result += wxS(' ');
				result += label;

				return true;
			}
		}
		return false;
	}
	
	template<class T>
	String FormatWithUnitBase(T value, BinarySizeFormat format, BinarySizeUnit unit, int precision)
	{
		String result;
		if (format & BinarySizeFormat::Fractional)
		{
			// Default precision is 2 digits
			result = String::FromCDouble(static_cast<double>(value), precision >= 0 ? precision : 2);
		}
		else
		{
			result = String::Format(wxS("%1"), static_cast<int64_t>(value));
		}
		AddUnitLabelIfNeeded(result, format, unit);

		return result;
	}
	String FormatWithUnit(BinarySize value, BinarySizeFormat format, BinarySizeUnit unit, int precision)
	{
		if (format & BinarySizeFormat::Fractional)
		{
			return FormatWithUnitBase(value.GetAsUnit<double>(unit), format, unit, precision);
		}
		else
		{
			return FormatWithUnitBase(value.GetAsUnit<int64_t>(unit), format, unit, precision);
		}
	}
}

namespace KxFramework
{
	String BinarySize::Format(BinarySizeUnit unit, BinarySizeFormat format, int precision) const
	{
		// Short-circuit for zero size
		if (IsNull())
		{
			return String::Format(wxS("0 %1"), GetSizeUnitString(BinarySizeUnit::Bytes));
		}

		switch (unit)
		{
			case BinarySizeUnit::Auto:
			{
				// https://programming.guide/worlds-most-copied-so-snippet.html
				// 0 for bytes, 1 for KB, 2 for MB and so on. Matches the values in 'BinarySizeUnit'.
				const int digitGroups = KxMath::Log(m_Value, 1024);
				const double value = m_Value / std::pow(1024, digitGroups);

				return FormatWithUnitBase(value, format, FromInt<BinarySizeUnit>(digitGroups), precision);
			}
			case BinarySizeUnit::Bytes:
			case BinarySizeUnit::KiloBytes:
			case BinarySizeUnit::MegaBytes:
			case BinarySizeUnit::GigaBytes:
			case BinarySizeUnit::TeraBytes:
			{
				return FormatWithUnit(*this, format, unit, precision);
			}
		};
		return {};
	}
}
