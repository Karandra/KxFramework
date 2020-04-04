#pragma once
#include "Common.h"
#include "ColorDefines.h"
#include "Kx/Utility/Numeric.h"
#include <wx/colour.h>
#include <wx/brush.h>
#include <wx/pen.h>
class wxWindow;

namespace KxFramework
{
	enum class C2SFormat
	{
		CSS,
		HTML,
	};
	enum class C2SAlpha
	{
		Auto,
		Never,
		Always,
	};
	enum class ColorSpace
	{
		None = 0,

		RGB,
		HSL,
		HSB
	};
}

namespace KxFramework
{
	class KX_API Color final
	{
		public:
			constexpr static Color FromHSB(const PackedHSB& color) noexcept
			{
				return Color().SetHSB(color);
			}
			constexpr static Color FromHSL(const PackedHSL& color) noexcept
			{
				return Color().SetHSL(color);
			}
			constexpr static Color FromRGBA(uint32_t color) noexcept
			{
				return Color().SetRGBA(color);
			}
			constexpr static Color FromARGB(uint32_t color) noexcept
			{
				return Color().SetARGB(color);
			}
			constexpr static Color FromCOLORREF(uint32_t color) noexcept
			{
				return Color().SetCOLORREF(color);
			}
			constexpr static Color FromFixed8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = ColorTraits<uint8_t>::max()) noexcept
			{
				return Color().SetFixed8(r, g, b, a);
			}
			constexpr static Color FromNormalized(float r, float g, float b, float a = ColorTraits<float>::max()) noexcept
			{
				return Color().SetNormalized(r, g, b, a);
			}
			static Color FromColorName(const wxString& name)
			{
				return wxColour(name);
			}

			// Range: [1, 21]
			constexpr static float ContrastRatio(const Color& lighterColor, const Color& darkerColor, const PackedRGB<float>& weight = ColorWeight::sRGB) noexcept
			{
				return (lighterColor.GetLuminance(weight) + 0.05f) / (darkerColor.GetLuminance(weight) + 0.05f);
			}

			constexpr static float AlphaBlend(float foreground, float background, float alpha) noexcept
			{
				using Traits = ColorTraits<float>;
				return std::clamp(background + (alpha * (foreground - background)), Traits::min(), Traits::max());
			}
			constexpr static Color AlphaBlend(const Color& foreground, const Color& background, float alpha) noexcept
			{
				return AlphaBlend(foreground.GetNormalized(), background.GetNormalized(), alpha);
			}
			constexpr static PackedRGBA<float> AlphaBlend(const PackedRGBA<float>& foreground, const PackedRGBA<float>& background, float alpha) noexcept
			{
				const float r = AlphaBlend(foreground.Red, background.Red, alpha);
				const float g = AlphaBlend(foreground.Green, background.Green, alpha);
				const float b = AlphaBlend(foreground.Blue, background.Blue, alpha);
				const float a = AlphaBlend(foreground.Alpha, background.Alpha, alpha);
				return {r, g, b, a};
			}

			constexpr static Color SelectLighterColor(const Color& left, const Color& right, const PackedRGB<float>& weight = ColorWeight::sRGB) noexcept
			{
				const float leftLuminance = left.GetLuminance(weight);
				const float rightLuminance = right.GetLuminance(weight);

				return leftLuminance > rightLuminance ? left : right;
			}
			constexpr static Color SelectDarkerColor(const Color& left, const Color& right, const PackedRGB<float>& weight = ColorWeight::sRGB) noexcept
			{
				const float leftLuminance = left.GetLuminance(weight);
				const float rightLuminance = right.GetLuminance(weight);

				return leftLuminance < rightLuminance ? left : right;
			}

		private:
			PackedRGBA<float> m_Value = {-1.0f, -1.0f, -1.0f, -1.0f};

		private:
			constexpr uint32_t GetABGR() const noexcept
			{
				auto temp = GetFixed8();
				return (static_cast<uint32_t>(temp.Alpha) << 24) | (temp.Red | (static_cast<uint16_t>(temp.Green) << 8)) | (static_cast<uint32_t>(temp.Blue) << 16);
			}
			constexpr Color& SetABGR(uint32_t color) noexcept
			{
				uint8_t a = ((color >> 24) & 0xff);
				uint8_t b = ((color >> 16) & 0xff);
				uint8_t g = ((color >> 8) & 0xff);
				uint8_t r = (color & 0xff);

				SetFixed8(r, g, b, a);
				return *this;
			}

		public:
			constexpr Color() noexcept = default;
			constexpr Color(Color&&) noexcept = default;
			constexpr Color(const Color&) noexcept = default;
			constexpr Color(const PackedRGBA<uint8_t>& other) noexcept
				:m_Value(ToNormalizedBBP(other))
			{
			}
			constexpr Color(const PackedRGBA<float>& other) noexcept
				:m_Value(other)
			{
			}
			Color(const wxColour& other) noexcept
			{
				if (other.IsOk())
				{
					m_Value = ToNormalizedBBP(other.Red(), other.Green(), other.Blue(), other.Alpha());
				}
			}

		public:
			// General
			constexpr bool IsValid() const noexcept
			{
				auto Test = [](float value) constexpr noexcept
				{
					using Traits = ColorTraits<float>;
					return value == std::clamp(value, Traits::min(), Traits::max());
				};
				return Test(m_Value.Red) && Test(m_Value.Green) && Test(m_Value.Blue) && Test(m_Value.Alpha);
			}
			constexpr bool IsOpaque() const noexcept
			{
				return m_Value.Alpha == 1.0f;
			}
			constexpr bool IsTransparent() const noexcept
			{
				return !IsOpaque();
			}
			constexpr Color Clone() const noexcept
			{
				return *this;
			}

			// Conversion to other types
			wxString ToString(C2SFormat format, C2SAlpha alpha = C2SAlpha::Auto, ColorSpace colorSpace = ColorSpace::RGB) const;
			wxColour ToWxColor() const noexcept
			{
				if (IsValid())
				{
					auto temp = GetFixed8();
					return wxColour(temp.Red, temp.Green, temp.Blue, temp.Alpha);
				}
				return {};
			}
			wxString GetColorName() const
			{
				return ToWxColor().GetAsString(wxC2S_NAME);
			}

			// Normalized
			constexpr PackedRGBA<float> GetNormalized() const noexcept
			{
				return m_Value;
			}
			constexpr Color& SetNormalized(const PackedRGBA<float>& other) noexcept
			{
				m_Value = other;
				return *this;
			}
			constexpr Color& SetNormalized(float r, float g, float b, float a = ColorTraits<float>::max()) noexcept
			{
				m_Value = PackedRGBA(r, g, b, a);
				return *this;
			}

			constexpr Color& SetRedNormalized(float r) noexcept
			{
				m_Value.Red = r;
				return *this;
			}
			constexpr Color& SetGreenNormalized(float g) noexcept
			{
				m_Value.Green = g;
				return *this;
			}
			constexpr Color& SetBlueNormalized(float b) noexcept
			{
				m_Value.Blue = b;
				return *this;
			}
			constexpr Color& SetAlphaNormalized(float a) noexcept
			{
				m_Value.Alpha = a;
				return *this;
			}

			// Fixed
			constexpr PackedRGBA<uint8_t> GetFixed8() const noexcept
			{
				return ToFixedBPP<uint8_t>(m_Value);
			}
			constexpr Color& SetFixed8(const PackedRGBA<uint8_t>& other) noexcept
			{
				m_Value = ToNormalizedBBP(other);
				return *this;
			}
			constexpr Color& SetFixed8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = ColorTraits<uint8_t>::max()) noexcept
			{
				m_Value = ToNormalizedBBP(r, g, b, a);
				return *this;
			}

			constexpr Color& SetRed8(uint8_t r) noexcept
			{
				m_Value.Red = Private::ChannelToNormalizedBBP(r);
				return *this;
			}
			constexpr Color& SetGreen8(uint8_t g) noexcept
			{
				m_Value.Green = Private::ChannelToNormalizedBBP(g);
				return *this;
			}
			constexpr Color& SetBlue8(uint8_t b) noexcept
			{
				m_Value.Blue = Private::ChannelToNormalizedBBP(b);
				return *this;
			}
			constexpr Color& SetAlpha8(uint8_t a) noexcept
			{
				m_Value.Alpha = Private::ChannelToNormalizedBBP(a);
				return *this;
			}

			// Color-space and representation conversions
			constexpr uint32_t GetRGBA() const noexcept
			{
				auto temp = GetFixed8();
				return ((temp.Red & 0xff) << 24) + ((temp.Green & 0xff) << 16) + ((temp.Blue & 0xff) << 8) + (temp.Alpha & 0xff);
			}
			constexpr Color& SetRGBA(uint32_t color) noexcept
			{
				if (((color >> 24) & 0xFF) == 0)
				{
					SetFixed8((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
				}
				else
				{
					SetFixed8((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
				}
				return *this;
			}
			
			constexpr uint32_t GetARGB() const noexcept
			{
				auto temp = GetFixed8();
				return ((temp.Alpha & 0xff) << 24) + ((temp.Red & 0xff) << 16) + ((temp.Green & 0xff) << 8) + (temp.Blue & 0xff);
			}
			constexpr Color& SetARGB(uint32_t color) noexcept
			{
				uint8_t a = ((color >> 24) & 0xff);
				uint8_t r = ((color >> 16) & 0xff);
				uint8_t g = ((color >> 8) & 0xff);
				uint8_t b = (color & 0xff);

				SetFixed8(r, g, b, a);
				return *this;
			}
			
			constexpr uint32_t GetCOLORREF() const noexcept
			{
				return GetABGR();
			}
			constexpr Color& SetCOLORREF(uint32_t color) noexcept
			{
				return SetABGR(color);
			}
			
			constexpr PackedHSB GetHSB() const noexcept
			{
				PackedHSB hsb;

				const float r = m_Value.Red;
				const float g = m_Value.Green;
				const float b = m_Value.Blue;

				const float max = std::max(std::max(r, g), b);
				const float min = std::min(std::min(r, g), b);
				const float delta = max - min;

				// Alpha
				hsb.Alpha = m_Value.Alpha;

				// Hue
				hsb.Hue = 0;
				if (max == r && g >= b)
				{
					hsb.Hue = 60.0f * ((g - b) / delta);
				}
				else if (max == r && g < b)
				{
					hsb.Hue = 60.0f * ((g - b) / delta) + 360.0f;
				}
				else if (max == g)
				{
					hsb.Hue = 60.0f * ((b - r)/ delta) + 120.0f;
				}
				else if (max == b)
				{
					hsb.Hue = 60.0f * ((r - g) / delta) + 240.0f;
				}

				// Saturation
				hsb.Saturation = 0;
				if (max != 0.0f)
				{
					hsb.Saturation = 1.0f - (min / max);
				}

				// Brightness
				hsb.Brightness = max;

				return hsb;
			}
			constexpr Color& SetHSB(PackedHSB hsb) noexcept
			{
				float n = 0;
				uint8_t Hi = Utility::Floor(hsb.Hue / 60.0f);
				Utility::ModF(Hi / 6.0f, &n);
				Hi = static_cast<uint8_t>(n);

				float Bmin = ((100.0f - hsb.Saturation) * hsb.Brightness) / 100.0f;
				Utility::ModF(hsb.Hue / 60.0f, &n);
				float a = (hsb.Brightness - Bmin) * (n / 60.0f);
				float Binc = Bmin + a;
				float Bdec = hsb.Brightness - a;

				const float rgb[][3] =
				{
					{hsb.Brightness, Binc, Bmin},
					{Bdec, hsb.Brightness, Bmin},
					{Bmin, hsb.Brightness, Binc},
					{Bmin, Bdec, hsb.Brightness},
					{Binc, Bmin, hsb.Brightness},
					{hsb.Brightness, Bmin, Bdec},
				};

				SetNormalized(rgb[Hi][0], rgb[Hi][1], rgb[Hi][2], hsb.Alpha);
				return *this;
			}

			constexpr PackedHSL GetHSL() const noexcept
			{
				PackedHSL hsl;

				double r = m_Value.Red;
				double g = m_Value.Green;
				double b = m_Value.Blue;

				const double max = std::max(std::max(r, g), b);
				const double min = std::min(std::min(r, g), b);
				const double delta = max - min;

				// Alpha
				hsl.Alpha = m_Value.Alpha;

				// Hue
				hsl.Hue = 0;
				if (max == r && g >= b)
				{
					hsl.Hue = 60.0 * ((g - b) / delta);
				}
				else if (max == r && g < b)
				{
					hsl.Hue = 60.0 * ((g - b) / delta) + 360.0;
				}
				else if (max == g)
				{
					hsl.Hue = 60.0 * ((b - r) / delta) + 120.0;
				}
				else if (max == b)
				{
					hsl.Hue = 60.0 * ((r - g) / delta) + 240.0;
				}

				// Saturation
				hsl.Saturation = delta / (1.0 - Utility::Abs((1.0 - (max + min))));

				// Lightness
				hsl.Lightness = 0.5 * (max + min);

				return hsl;
			}
			constexpr Color& SetHSL(PackedHSL hsl) noexcept
			{
				hsl.Hue /= 360.0f;
				hsl.Saturation /= 100.0f;
				hsl.Lightness /= 100.0f;

				PackedRGBA<float> rgb;
				if (hsl.Saturation == 0)
				{
					rgb.Red = hsl.Lightness;
					rgb.Green = hsl.Lightness;
					rgb.Blue = hsl.Lightness;
				}
				else
				{
					const float q = hsl.Lightness < 0.5f ? hsl.Lightness * (1.0f + hsl.Saturation) : hsl.Lightness + hsl.Saturation - hsl.Lightness * hsl.Saturation;
					const float p = 2.0f * hsl.Lightness - q;

					auto HUE2RGB = [](float p, float q, float t) constexpr
					{
						if (t < 0)
						{
							t += 1.0f;
						}
						if (t > 1)
						{
							t -= 1.0f;
						}
						if (t < 1.0f / 6.0f)
						{
							return p + (q - p) * 6.0f * t;
						}
						if (t < 1.0f / 2.0f)
						{
							return q;
						}
						if (t < 2.0f / 3.0f)
						{
							return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
						}
						return p;
					};
					rgb.Red = HUE2RGB(p, q, hsl.Hue + (1.0f / 3.0f));
					rgb.Green = HUE2RGB(p, q, hsl.Hue);
					rgb.Blue = HUE2RGB(p, q, hsl.Hue - (1.0f / 3.0f));
				}
				rgb.Alpha = hsl.Alpha;

				SetNormalized(rgb);
				return *this;
			}
			
			// Operations
			constexpr Color MakeMono(bool isOn = true) const noexcept
			{
				using Traits = ColorTraits<float>;

				const float value = isOn ? Traits::max() : Traits::min();
				return FromNormalized(value, value, value, m_Value.Alpha);
			}
			constexpr Color MakeGray(const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				// https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
				const float luma = m_Value.Red * weight.Red + m_Value.Green * weight.Green + m_Value.Blue * weight.Blue;
				return FromNormalized(luma, luma, luma, m_Value.Alpha);
			}
			constexpr Color MakeDisabled(float brightness = 1.0f) const noexcept
			{
				constexpr float alpha = 0.4f;

				auto temp = GetNormalized();
				temp.Red = AlphaBlend(temp.Red, brightness, alpha);
				temp.Green = AlphaBlend(temp.Green, brightness, alpha);
				temp.Blue = AlphaBlend(temp.Blue, brightness, alpha);

				return temp;
			}
			constexpr Color ChangeLightness(float alpha) const noexcept
			{
				if (alpha != 0.5f)
				{
					using Traits = ColorTraits<float>;

					float background = 0;
					if (alpha > 0.5f)
					{
						// Blend with white
						background = Traits::max();
						alpha = 1.0f - alpha; 
					}
					else
					{
						// Blend with black
						background = 0;
						alpha = 1.0f + alpha;
					}

					auto temp = GetNormalized();
					temp.Red = AlphaBlend(temp.Red, background, alpha);
					temp.Green = AlphaBlend(temp.Green, background, alpha);
					temp.Blue = AlphaBlend(temp.Blue, background, alpha);
					return temp;
				}
				return *this;
			}
			constexpr Color RotateHue(double angle) const noexcept
			{
				PackedHSL hsl = GetHSL();

				hsl.Hue += angle;
				if (hsl.Hue > 1.0f)
				{
					hsl.Hue -= 1.0f;
				}
				else if (hsl.Hue < 0.0f)
				{
					hsl.Hue += 1.0f;
				}
				return Color().SetHSL(hsl);
			}
			constexpr Color Invert() const noexcept
			{
				PackedHSL hsl = GetHSL();
				hsl.Hue = static_cast<int>(hsl.Hue + 180.0f) % 360;

				return Color().SetHSL(hsl);
			}
			constexpr Color Negate() const noexcept
			{
				return FromRGBA(0xFFFFFF00u ^ GetRGBA());
			}
			constexpr Color GetContrastColor(const Color& light, const Color& dark, const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				// https://stackoverflow.com/questions/7260989/how-to-pick-good-contrast-rgb-colors-programmatically/7261283#7261283
				return GetLuminance(weight) < 0.5f ? light : dark;
			}
			Color GetContrastColor(const wxWindow& window, const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept;

			constexpr float GetDifference(const Color& other) const noexcept
			{
				const float r1 = m_Value.Red;
				const float g1 = m_Value.Green;
				const float b1 = m_Value.Blue;

				const float r2 = other.m_Value.Red;
				const float g2 = other.m_Value.Green;
				const float b2 = other.m_Value.Blue;

				return (std::max(r1, r2) - std::min(r1, r2)) + (std::max(g1, g2) - std::min(g1, g2)) + (std::max(b1, b2) - std::min(b1, b2));
			}
			constexpr float GetLuminance(const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				// https://stackoverflow.com/a/56678483/6512579
				auto SRGBToLinear = [](float channel) constexpr noexcept
				{
					// Send this function a decimal sRGB gamma encoded color value
					// between 0.0 and 1.0, and it returns a linearized value.

					if (channel <= 0.04045f)
					{
						return channel / 12.92f;
					}
					else
					{
						return std::pow(((channel + 0.055f) / 1.055f), 2.4f);
					}
				};

				const float r = SRGBToLinear(m_Value.Red);
				const float g = SRGBToLinear(m_Value.Green);
				const float b = SRGBToLinear(m_Value.Blue);

				return r * weight.Red + g * weight.Green + b * weight.Blue;
			}
			constexpr float GetPercievedLighteness(const PackedRGB<float>& weight = ColorWeight::sRGB) const noexcept
			{
				auto YToLstar = [](float luminance) constexpr noexcept
				{
					// Send this function a luminance value between 0.0 and 1.0,
					// and it returns L* which is "perceptual lightness"

					// The CIE standard states 0.008856 but 216/24389 is the intent for 0.008856451679036
					if (luminance <= (216.0f / 24389.0f))
					{
						// The CIE standard states 903.3, but 24389/27 is the intent, making 903.296296296296296
						return luminance * (24389.0f / 27.0f);
					}
					else
					{
						return std::pow(luminance, (1.0f / 3.0f)) * 116.0f - 16.0f;
					}
				};
				return YToLstar(GetLuminance(weight));
			}

		public:
			constexpr explicit operator bool() const
			{
				return IsValid();
			}
			constexpr bool operator!() const
			{
				return !IsValid();
			}
			
			constexpr bool operator==(const Color& other) const noexcept
			{
				return this == &other || m_Value == other.m_Value;
			}
			constexpr bool operator==(const wxColour& other) const noexcept
			{
				if (IsValid() && other.IsOk())
				{
					return GetFixed8() == PackedRGBA<uint8_t>(other.Red(), other.Green(), other.Blue(), other.Alpha());
				}
				return false;
			}
			constexpr bool operator!=(const Color& other) const noexcept
			{
				return !(*this == other);
			}
			constexpr bool operator!=(const wxColour& other) const noexcept
			{
				return !(*this == other);
			}

			constexpr Color& operator=(Color&&) noexcept = default;
			constexpr Color& operator=(const Color&) noexcept = default;
			Color& operator=(const wxColour& other) noexcept
			{
				*this = Color(other);
				return *this;
			}

			operator wxColour() const noexcept
			{
				return ToWxColor();
			}
			operator wxBrush() const noexcept
			{
				return ToWxColor();
			}
			operator wxPen() const noexcept
			{
				return ToWxColor();
			}
	};
}
