#include "stdafx.h"
#include "BinarySize.h"
#include "StringFormater.h"
#include "Math.h"
#include "kxf/Application/ICoreApplication.h"

namespace
{
	using namespace kxf;

	std::optional<String> GetSizeUnitString(BinarySizeUnit unit)
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			const ILocalizationPackage& localizationPackage = app->GetLocalizationPackage();
			switch (unit)
			{
				case BinarySizeUnit::Bytes:
				{
					return localizationPackage.GetItem(wxS("BinarySizeUnit.Bytes"));
				}
				case BinarySizeUnit::KiloBytes:
				{
					return localizationPackage.GetItem(wxS("BinarySizeUnit.KB"));
				}
				case BinarySizeUnit::MegaBytes:
				{
					return localizationPackage.GetItem(wxS("BinarySizeUnit.MB"));
				}
				case BinarySizeUnit::GigaBytes:
				{
					return localizationPackage.GetItem(wxS("BinarySizeUnit.GB"));
				}
				case BinarySizeUnit::TeraBytes:
				{
					return localizationPackage.GetItem(wxS("BinarySizeUnit.TB"));
				}
			};
		}
		return {};
	}
	bool AddUnitLabelIfNeeded(String& result, FlagSet<BinarySizeFormat> format, BinarySizeUnit unit)
	{
		if (format & BinarySizeFormat::WithLabel && !result.IsEmpty())
		{
			if (auto label = GetSizeUnitString(unit))
			{
				result += wxS(' ');
				result += *label;

				return true;
			}
		}
		return false;
	}
	
	template<class T>
	String FormatWithUnitBase(T value, FlagSet<BinarySizeFormat> format, BinarySizeUnit unit, int precision)
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
	
	String FormatWithUnit(BinarySize value, FlagSet<BinarySizeFormat> format, BinarySizeUnit unit, int precision)
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

namespace kxf
{
	String BinarySize::Format(BinarySizeUnit unit, FlagSet<BinarySizeFormat> format, int precision) const
	{
		// Short-circuit for zero
		if (IsNull())
		{
			if (auto label = GetSizeUnitString(BinarySizeUnit::Bytes))
			{
				return String::Format(wxS("0 %1"), *label);
			}
			return wxS("0");
		}

		switch (unit)
		{
			case BinarySizeUnit::Auto:
			{
				// https://programming.guide/worlds-most-copied-so-snippet.html
				// 0 for bytes, 1 for KB, 2 for MB and so on. Matches the values in 'BinarySizeUnit'.
				const int digitGroups = Math::Log(m_Value, 1024);
				const double value = m_Value / std::pow(1024, digitGroups);

				return FormatWithUnitBase(value, format, static_cast<BinarySizeUnit>(digitGroups), precision);
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
