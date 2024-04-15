#include "KxfPCH.h"
#include "BinarySize.h"
#include "Math.h"
#include "Format.h"
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
					return localizationPackage.GetItem("BinarySizeUnit.Bytes");
				}
				case BinarySizeUnit::KiloBytes:
				{
					return localizationPackage.GetItem("BinarySizeUnit.KB");
				}
				case BinarySizeUnit::MegaBytes:
				{
					return localizationPackage.GetItem("BinarySizeUnit.MB");
				}
				case BinarySizeUnit::GigaBytes:
				{
					return localizationPackage.GetItem("BinarySizeUnit.GB");
				}
				case BinarySizeUnit::TeraBytes:
				{
					return localizationPackage.GetItem("BinarySizeUnit.TB");
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
				result += ' ';
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
			result = String::FromFloatingPoint(static_cast<double>(value), precision >= 0 ? precision : 2);
		}
		else
		{
			result = kxf::ToString(value);
		}
		AddUnitLabelIfNeeded(result, format, unit);

		return result;
	}
	
	String FormatWithUnit(BinarySize value, FlagSet<BinarySizeFormat> format, BinarySizeUnit unit, int precision)
	{
		if (format & BinarySizeFormat::Fractional)
		{
			return FormatWithUnitBase(value.ToUnit<double>(unit), format, unit, precision);
		}
		else
		{
			return FormatWithUnitBase(value.ToUnit<int64_t>(unit), format, unit, precision);
		}
	}
}

namespace kxf
{
	String BinarySize::ToString(BinarySizeUnit unit, FlagSet<BinarySizeFormat> format, int precision) const
	{
		// Short-circuit for zero
		if (IsNull())
		{
			if (auto label = GetSizeUnitString(BinarySizeUnit::Bytes))
			{
				return Format("0 {}", *label);
			}
			return "0";
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
