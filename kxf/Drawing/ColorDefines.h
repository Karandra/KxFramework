#pragma once
#include "Angle.h"
#include "kxf/Utility/Numeric.h"

namespace kxf
{
	enum class StockColor
	{
		Transparent,

		Black,
		White,
		Cyan,
		Blue,
		Red,
		Green,
		Yellow,
		Gray,
		LightGray,
		MediumGray,
		Purple,
		Magenta
	};
}

namespace kxf
{
	template<class T, bool isInteger = std::is_integral_v<T> && std::is_unsigned_v<T>, bool isFloatingPoint = std::is_floating_point_v<T>>
	struct ColorTraits
	{
	};

	template<class T>
	struct ColorTraits<T, true, false>
	{
		static constexpr T min() noexcept
		{
			return std::numeric_limits<T>::min();
		}
		static constexpr T max() noexcept
		{
			return std::numeric_limits<T>::max();
		}

		static constexpr T clamp(T value) noexcept
		{
			return std::clamp(value, min(), max());
		}
		static constexpr bool test_range(T value) noexcept
		{
			return value == clamp(value);
		}
	};

	template<class T>
	struct ColorTraits<T, false, true>
	{
		static constexpr T min() noexcept
		{
			return 0;
		}
		static constexpr T max() noexcept
		{
			return 1;
		}

		static constexpr T clamp(T value) noexcept
		{
			return std::clamp(value, min(), max());
		}
		static constexpr bool test_range(T value) noexcept
		{
			return value == clamp(value);
		}
	};
}

namespace kxf
{
	template<class T>
	struct PackedRGBA;

	template<class T>
	struct PackedRGB final
	{
		public:
			using ValueType = T;

		public:
			T Red = 0;
			T Green = 0;
			T Blue = 0;

		public:
			constexpr PackedRGB() noexcept = default;
			constexpr PackedRGB(T r, T g, T b) noexcept
				:Red(r), Green(g), Blue(b)
			{
			}

		public:
			constexpr PackedRGBA<T> AddAlpha(T a) const noexcept
			{
				return {Red, Green, Blue, a};
			}

			template<class T>
			constexpr PackedRGB<T> Cast() const noexcept
			{
				return {static_cast<T>(Red), static_cast<T>(Green), static_cast<T>(Blue)};
			}

		public:
			constexpr bool operator==(const PackedRGB& other) const noexcept
			{
				if (this != &other)
				{
					if constexpr(std::is_floating_point_v<T>)
					{
						using namespace Utility;
						return AlmostEqual(Red, other.Red) && AlmostEqual(Green, other.Green) && AlmostEqual(Blue, other.Blue);
					}
					else
					{
						return Red == other.Red && Green == other.Green && Blue == other.Blue;
					}
				}
				return true;
			}
			constexpr bool operator!=(const PackedRGB& other) const noexcept
			{
				return !(*this == other);
			}
	};

	template<class T>
	struct PackedRGBA final
	{
		public:
			using ValueType = T;

		public:
			T Red = 0;
			T Green = 0;
			T Blue = 0;
			T Alpha = 0;

		public:
			constexpr PackedRGBA() noexcept = default;
			constexpr PackedRGBA(T r, T g, T b, T a = ColorTraits<T>::max()) noexcept
				:Red(r), Green(g), Blue(b), Alpha(a)
			{
			}
			constexpr PackedRGBA(const PackedRGB<T>& other, T a = ColorTraits<T>::max()) noexcept
				:Red(other.Red), Green(other.Green), Blue(other.Blue), Alpha(a)
			{
			}

		public:
			constexpr bool IsSameAs(const PackedRGB<T>& other) const noexcept
			{
				if constexpr(std::is_floating_point_v<T>)
				{
					using namespace Utility;
					return AlmostEqual(Red, other.Red) && AlmostEqual(Green, other.Green) && AlmostEqual(Blue, other.Blue);
				}
				else
				{
					return Red == other.Red && Green == other.Green && Blue == other.Blue;
				}
			}
			constexpr bool IsSameAs(const PackedRGBA& other) const noexcept
			{
				if (this != &other)
				{
					if constexpr(std::is_floating_point_v<T>)
					{
						using namespace Utility;
						return AlmostEqual(Red, other.Red) && AlmostEqual(Green, other.Green) && AlmostEqual(Blue, other.Blue);
					}
					else
					{
						return Red == other.Red && Green == other.Green && Blue == other.Blue;
					}
				}
				return true;
			}

			constexpr PackedRGB<T> RemoveAlpha() const noexcept
			{
				return {Red, Green, Blue};
			}

			template<class T>
			constexpr PackedRGBA<T> Cast() const noexcept
			{
				return {static_cast<T>(Red), static_cast<T>(Green), static_cast<T>(Blue), static_cast<T>(Alpha)};
			}

		public:
			constexpr bool operator==(const PackedRGBA& other) const noexcept
			{
				return IsSameAs(other);
			}
			constexpr bool operator!=(const PackedRGBA& other) const noexcept
			{
				return !(*this == other);
			}
	};
}

namespace kxf::ColorWeight
{
	// https://ninedegreesbelow.com/photography/srgb-luminance.html
	// https://en.wikipedia.org/wiki/Luma_(video)

	constexpr PackedRGB<float> sRGB = {0.2126f, 0.7152f, 0.0722f};
	constexpr PackedRGB<float> CCIR_601 = {0.299f, 0.587f, 0.114f};
	constexpr PackedRGB<float> ITU_R = {0.2126f, 0.7152f, 0.0722f};
	constexpr PackedRGB<float> HDTV = {0.212f, 0.701f, 0.087f};
}

namespace kxf
{
	struct PackedHSL final
	{
		using ValueType = float;

		Angle Hue = Angle::FromNormalized(0);
		float Saturation = 0;
		float Lightness = 0;
		float Alpha = 0;
	};
	struct PackedHSV final
	{
		using ValueType = float;

		Angle Hue = Angle::FromNormalized(0);
		float Saturation = 0;
		float Value = 0;
		float Alpha = 0;
	};

	inline constexpr PackedHSL ToHSL(const PackedHSV& hsv) noexcept
	{
		// https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_HSL
		PackedHSL hsl;
		hsl.Alpha = hsv.Alpha;
		hsl.Hue = hsv.Hue;
		hsl.Lightness = hsv.Value * (1.0f - hsv.Saturation / 2.0f);

		if (hsl.Lightness == 0.0f || hsl.Lightness == 1.0f)
		{
			hsl.Saturation = 0.0f;
		}
		else
		{
			hsl.Saturation = (hsv.Value - hsl.Lightness) / std::min(hsl.Lightness, 1.0f - hsl.Lightness);
		}
		return hsl;
	}
	inline constexpr PackedHSV ToHSV(const PackedHSL& hsl) noexcept
	{
		// https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_HSV
		PackedHSV hsv;
		hsv.Alpha = hsl.Alpha;
		hsv.Hue = hsl.Hue;
		hsv.Value = hsl.Lightness + hsl.Saturation * std::min(hsl.Lightness, 1.0f - hsl.Lightness);
		hsv.Saturation = hsv.Value == 0.0f ? 0 : 2.0f * (1.0f - hsl.Lightness / hsv.Value);

		return hsv;
	}
}

namespace kxf
{
	namespace Private
	{
		template<class T, class Ta>
		constexpr T ChannelToFixedBPP(Ta value) noexcept
		{
			static_assert(std::is_floating_point_v<Ta>);

			return value * ColorTraits<T>::max();
		}

		template<class T>
		constexpr float ChannelToNormalizedBBP(T value) noexcept
		{
			static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);

			return value / static_cast<float>(ColorTraits<T>::max());
		}
	}

	template<class T, class Ta>
	constexpr PackedRGBA<T> ToFixedBPP(Ta r, Ta g, Ta b, Ta a = ColorTraits<Ta>::max()) noexcept
	{
		using namespace Private;

		return {ChannelToFixedBPP<T>(r), ChannelToFixedBPP<T>(g), ChannelToFixedBPP<T>(b), ChannelToFixedBPP<T>(a)};
	}

	template<class T, class Ta>
	constexpr PackedRGBA<T> ToFixedBPP(const PackedRGBA<Ta>& color) noexcept
	{
		return ToFixedBPP<T>(color.Red, color.Green, color.Blue, color.Alpha);
	}

	template<class T>
	constexpr PackedRGBA<float> ToNormalizedBBP(T r, T g, T b, T a = ColorTraits<T>::max()) noexcept
	{
		using namespace Private;

		return {ChannelToNormalizedBBP(r), ChannelToNormalizedBBP(g), ChannelToNormalizedBBP(b), ChannelToNormalizedBBP(a)};
	}

	template<class T>
	constexpr PackedRGBA<float> ToNormalizedBBP(const PackedRGBA<T>& color) noexcept
	{
		return ToNormalizedBBP(color.Red, color.Green, color.Blue, color.Alpha);
	}
}
