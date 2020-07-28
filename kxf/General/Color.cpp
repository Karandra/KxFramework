#include "stdafx.h"
#include "Color.h"
#include "StringFormater.h"
#include "wx/window.h"

namespace
{
	kxf::String CSS2RGB(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto fixed = color.GetFixed8();
				return String::Format(wxS("rgb(%1, %2, %3)"), fixed.Red, fixed.Green, fixed.Blue);
			}
			case C2SAlpha::Always:
			{
				const auto fixed = color.GetFixed8();
				const auto normalized = color.GetNormalized();
				return String::Format(wxS("rgba(%1, %2, %3, %4)"), fixed.Red, fixed.Green, fixed.Blue, normalized.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? CSS2RGB(color, C2SAlpha::Never) : CSS2RGB(color, C2SAlpha::Always);
			}
		};
		return {};
	}
	kxf::String CSS2HSL(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto hsl = color.GetHSL();
				return String::Format(wxS("hsl(%1, %2, %3)"), hsl.Hue.ToDegrees(), hsl.Saturation, hsl.Lightness);
			}
			case C2SAlpha::Always:
			{
				const auto hsl = color.GetHSL();
				const auto normalized = color.GetNormalized();
				return String::Format(wxS("hsla(%1, %2, %3, %4)"), hsl.Hue.ToDegrees(), hsl.Saturation, hsl.Lightness, normalized.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? CSS2HSL(color, C2SAlpha::Never) : CSS2HSL(color, C2SAlpha::Always);
			}
		};

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto fixed = color.GetFixed8();
				return String::Format(wxS("rgb(%1, %2, %3)"), fixed.Red, fixed.Green, fixed.Blue);
			}
			case C2SAlpha::Always:
			{
				const auto fixed = color.GetFixed8();
				const auto normalized = color.GetNormalized();
				return String::Format(wxS("rgba(%1, %2, %3, %4)"), fixed.Red, fixed.Green, fixed.Blue, normalized.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? CSS2RGB(color, C2SAlpha::Never) : CSS2RGB(color, C2SAlpha::Always);
			}
		};
		return {};
	}

	kxf::String HTML2RGB(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto fixed = color.GetFixed8();

				StringFormatter::Formatter formatter(wxS("#%1%2%3"));
				formatter(fixed.Red, 2, 16, '0');
				formatter(fixed.Green, 2, 16, '0');
				formatter(fixed.Blue, 2, 16, '0');
				return formatter;
			}
			case C2SAlpha::Always:
			{
				const auto fixed = color.GetFixed8();

				StringFormatter::Formatter formatter(wxS("#%1%2%3%4"));
				formatter(fixed.Red, 2, 16, '0');
				formatter(fixed.Green, 2, 16, '0');
				formatter(fixed.Blue, 2, 16, '0');
				formatter(fixed.Alpha, 2, 16, '0');
				return formatter;
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? HTML2RGB(color, C2SAlpha::Never) : HTML2RGB(color, C2SAlpha::Always);
			}
		};
		return {};
	}
	kxf::String HTML2HSL(const kxf::Color& color, kxf::C2SAlpha alpha)
	{
		using namespace kxf;

		switch (alpha)
		{
			case C2SAlpha::Never:
			{
				const auto hsl = color.GetHSL();
				return String::Format(wxS("hsl(%1, %2%, %3%)"), static_cast<int>(hsl.Hue.ToDegrees()), static_cast<int>(hsl.Saturation * 100), static_cast<int>(hsl.Lightness * 100));
			}
			case C2SAlpha::Always:
			{
				const auto hsl = color.GetHSL();
				return String::Format(wxS("hsl(%1, %2%, %3%, %4)"), static_cast<int>(hsl.Hue.ToDegrees()), static_cast<int>(hsl.Saturation * 100), static_cast<int>(hsl.Lightness * 100), hsl.Alpha);
			}
			case C2SAlpha::Auto:
			{
				return color.IsOpaque() ? HTML2HSL(color, C2SAlpha::Never) : HTML2HSL(color, C2SAlpha::Always);
			}
		};
		return {};
	}
}

namespace kxf
{
	Color Color::FromColorName(const String& name)
	{
		return wxTheColourDatabase->Find(name.GetWxString());
	}

	String Color::ToString(C2SFormat format, C2SAlpha alpha, ColorSpace colorSpace) const
	{
		switch (format)
		{
			case C2SFormat::CSS:
			{
				switch (colorSpace)
				{
					case ColorSpace::RGB:
					{
						return CSS2RGB(*this, alpha);
					}
					case ColorSpace::HSL:
					{
						return CSS2HSL(*this, alpha);
					}
				};
				break;
			}
			case C2SFormat::HTML:
			{
				switch (colorSpace)
				{
					case ColorSpace::RGB:
					{
						return HTML2RGB(*this, alpha);
					}
					case ColorSpace::HSL:
					{
						return HTML2HSL(*this, alpha);
					}
				};
				break;
			}
		};
		return {};
	}
	String Color::GetColorName() const
	{
		const auto fixed8 = GetFixed8();
		return wxTheColourDatabase->FindName(wxColour(fixed8.Red, fixed8.Green, fixed8.Blue, wxALPHA_OPAQUE));
	}

	Color Color::GetContrastColor(const wxWindow& window, const PackedRGB<float>& weight) const noexcept
	{
		auto [light, dark] = SelectLighterAndDarkerColor(window.GetBackgroundColour(), window.GetForegroundColour());
		return GetContrastColor(light, dark);
	}
}
