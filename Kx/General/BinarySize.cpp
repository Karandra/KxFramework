#include "stdafx.h"
#include "BinarySize.h"
#include "StringFormater.h"
#include "Math.h"
#include "Kx/Localization/LocalizationPackage.h"

namespace
{
	std::optional<KxFramework::String> GetSizeUnitString(KxFramework::BinarySizeUnit unit)
	{
		using namespace KxFramework;

		switch (unit)
		{
			case BinarySizeUnit::Bytes:
			{
				return LocalizationPackage::GetActive().GetString(wxS("BinarySizeUnit.Bytes"));
			}
			case BinarySizeUnit::KiloBytes:
			{
				return LocalizationPackage::GetActive().GetString(wxS("BinarySizeUnit.KB"));
			}
			case BinarySizeUnit::MegaBytes:
			{
				return LocalizationPackage::GetActive().GetString(wxS("BinarySizeUnit.MB"));
			}
			case BinarySizeUnit::GigaBytes:
			{
				return LocalizationPackage::GetActive().GetString(wxS("BinarySizeUnit.GB"));
			}
			case BinarySizeUnit::TeraBytes:
			{
				return LocalizationPackage::GetActive().GetString(wxS("BinarySizeUnit.TB"));
			}
		};
		return {};
	}
	bool AddUnitLabelIfNeeded(KxFramework::String& result, KxFramework::BinarySizeFormat format, KxFramework::BinarySizeUnit unit)
	{
		using namespace KxFramework;

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
	KxFramework::String FormatWithUnitBase(T value, KxFramework::BinarySizeFormat format, KxFramework::BinarySizeUnit unit, int precision)
	{
		using namespace KxFramework;

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
	
	KxFramework::String FormatWithUnit(KxFramework::BinarySize value, KxFramework::BinarySizeFormat format, KxFramework::BinarySizeUnit unit, int precision)
	{
		using namespace KxFramework;

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
