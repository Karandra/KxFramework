#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include <wx/font.h>

namespace kxf
{
	enum class FontFamily
	{
		None = wxFONTFAMILY_UNKNOWN,

		Default = wxFONTFAMILY_DEFAULT,
		Fantasy = wxFONTFAMILY_DECORATIVE,
		Serif = wxFONTFAMILY_ROMAN,
		SansSerif = wxFONTFAMILY_SWISS,
		Cursive = wxFONTFAMILY_SCRIPT,
		FixedWidth = wxFONTFAMILY_TELETYPE
	};
	enum class FontWeight
	{
		None = wxFONTWEIGHT_INVALID,

		Thin = wxFONTWEIGHT_THIN,
		ExtraLight = wxFONTWEIGHT_EXTRALIGHT,
		Light = wxFONTWEIGHT_LIGHT,
		Normal = wxFONTWEIGHT_NORMAL,
		Medium = wxFONTWEIGHT_MEDIUM,
		SemiBold = wxFONTWEIGHT_SEMIBOLD,
		Bold = wxFONTWEIGHT_BOLD,
		ExtraBold = wxFONTWEIGHT_EXTRABOLD,
		Heavy = wxFONTWEIGHT_HEAVY,
		ExtraHeavy = wxFONTWEIGHT_EXTRAHEAVY,
	};
	enum class FontSymbolicSize
	{
		ExtraSmall = wxFONTSIZE_XX_SMALL,
		VerySmall = wxFONTSIZE_X_SMALL,
		Small = wxFONTSIZE_SMALL,
		Normal = wxFONTSIZE_MEDIUM,
		Large = wxFONTSIZE_LARGE,
		VeryLarge = wxFONTSIZE_X_LARGE,
		ExtraLarge = wxFONTSIZE_XX_LARGE,
	};

	enum class FontStyle: uint32_t
	{
		Normal = 0,

		Italic = 1 << 1,
		Oblique = 1 << 2,
		Underline = 1 << 3,
		Strikethrough = 1 << 4,
	};
	KxFlagSet_Declare(FontStyle);

	enum class FontEncoding
	{
		None = wxFONTENCODING_MAX,
		System = wxFONTENCODING_SYSTEM,
		Default = wxFONTENCODING_DEFAULT,

		UTF7 = wxFONTENCODING_UTF7,
		UTF8 = wxFONTENCODING_UTF8,

		UTF16LE = wxFONTENCODING_UTF16LE,
		UTF16BE = wxFONTENCODING_UTF16BE,
		UTF32LE = wxFONTENCODING_UTF32LE,
		UTF32BE = wxFONTENCODING_UTF32BE,

		ISO8859_1 = wxFONTENCODING_ISO8859_1,
		ISO8859_2 = wxFONTENCODING_ISO8859_2,
		ISO8859_3 = wxFONTENCODING_ISO8859_3,
		ISO8859_4 = wxFONTENCODING_ISO8859_4,
		ISO8859_5 = wxFONTENCODING_ISO8859_5,
		ISO8859_6 = wxFONTENCODING_ISO8859_6,
		ISO8859_7 = wxFONTENCODING_ISO8859_7,
		ISO8859_8 = wxFONTENCODING_ISO8859_8,
		ISO8859_9 = wxFONTENCODING_ISO8859_9,
		ISO8859_10 = wxFONTENCODING_ISO8859_10,
		ISO8859_11 = wxFONTENCODING_ISO8859_11,
		ISO8859_12 = wxFONTENCODING_ISO8859_12,
		ISO8859_13 = wxFONTENCODING_ISO8859_13,
		ISO8859_14 = wxFONTENCODING_ISO8859_14,
		ISO8859_15 = wxFONTENCODING_ISO8859_15,
		ISO2022_JP = wxFONTENCODING_ISO2022_JP,

		KIO8R = wxFONTENCODING_KOI8,
		KIO8U = wxFONTENCODING_KOI8_U,
		GB2312 = wxFONTENCODING_GB2312,
		BIG5 = wxFONTENCODING_BIG5,
		Johab = wxFONTENCODING_JOHAB,
		EUC_KR = wxFONTENCODING_EUC_KR,
		EUC_JP = wxFONTENCODING_EUC_JP,
		ShiftJIS = wxFONTENCODING_SHIFT_JIS,
		Bulgarian = wxFONTENCODING_BULGARIAN,
		Vietnamese = wxFONTENCODING_VIETNAMESE,

		CP437 = wxFONTENCODING_CP437,
		CP850 = wxFONTENCODING_CP850,
		CP852 = wxFONTENCODING_CP852,
		CP855 = wxFONTENCODING_CP855,
		CP866 = wxFONTENCODING_CP866,
		CP874 = wxFONTENCODING_CP874,
		CP932 = wxFONTENCODING_CP932,
		CP936 = wxFONTENCODING_CP936,
		CP949 = wxFONTENCODING_CP949,
		CP950 = wxFONTENCODING_CP950,
		CP1250 = wxFONTENCODING_CP1250,
		CP1251 = wxFONTENCODING_CP1251,
		CP1252 = wxFONTENCODING_CP1252,
		CP1253 = wxFONTENCODING_CP1253,
		CP1254 = wxFONTENCODING_CP1254,
		CP1255 = wxFONTENCODING_CP1255,
		CP1256 = wxFONTENCODING_CP1256,
		CP1257 = wxFONTENCODING_CP1257,
		CP1258 = wxFONTENCODING_CP1258,
		CP1361 = wxFONTENCODING_CP1361,
	};

	struct FontMetrics final
	{
		public:
			int Height = 0; // Total character height.
			int Ascent = 0; // Part of the height above the baseline.
			int Descent = 0; // Part of the height below the baseline.
			int AverageWidth = 0; // Average font width or "x-width".
			int InternalLeading = 0; // Intra-line spacing.
			int ExternalLeading = 0; // Inter-line spacing.

		public:
			constexpr FontMetrics() noexcept = default;
			constexpr FontMetrics(const wxFontMetrics& other) noexcept
				:Height(other.height), Ascent(other.ascent), Descent(other.descent),
				AverageWidth(other.averageWidth), InternalLeading(other.internalLeading), ExternalLeading(other.externalLeading)
			{
			}

		public:
			operator wxFontMetrics() const noexcept
			{
				wxFontMetrics fontMetrics;
				fontMetrics.height = Height;
				fontMetrics.ascent = Ascent;
				fontMetrics.descent = Descent;
				fontMetrics.averageWidth = AverageWidth;
				fontMetrics.internalLeading = InternalLeading;
				fontMetrics.externalLeading = ExternalLeading;
			}

	};
}

namespace kxf::Private
{
	constexpr wxFontStyle MapFontStyle(FlagSet<FontStyle> style) noexcept
	{
		if (style.Contains(FontStyle::Normal))
		{
			return wxFONTSTYLE_NORMAL;
		}
		if (style.Contains(FontStyle::Italic))
		{
			return wxFONTSTYLE_ITALIC;
		}
		if (style.Contains(FontStyle::Oblique))
		{
			return wxFONTSTYLE_SLANT;
		}
		return wxFONTSTYLE_MAX;
	}
	constexpr FlagSet<FontStyle> MapFontStyle(wxFontStyle style) noexcept
	{
		switch (style)
		{
			case wxFONTSTYLE_NORMAL:
			{
				return FontStyle::Normal;
			}
			case wxFONTSTYLE_ITALIC:
			{
				return FontStyle::Italic;
			}
			case wxFONTSTYLE_SLANT:
			{
				return FontStyle::Oblique;
			}
		};
		return {};
	}
}

namespace kxf
{
	class KX_API Font: public RTTI::ExtendInterface<Font, IGDIObject>
	{
		KxRTTI_DeclareIID(Font, {0xbeb3a65c, 0xf639, 0x4e44, {0x80, 0x3a, 0x1b, 0x53, 0xf6, 0x9c, 0x61, 0xd8}});

		public:
			static FontEncoding GetDefaultEncoding() noexcept
			{
				return static_cast<FontEncoding>(wxFont::GetDefaultEncoding());
			}
			static void SetDefaultEncoding(FontEncoding encoding) noexcept
			{
				wxFont::SetDefaultEncoding(static_cast<wxFontEncoding>(encoding));
			}

			static int GetNumericWeightOf(FontWeight weight) noexcept
			{
				return wxFont::GetNumericWeightOf(static_cast<wxFontWeight>(weight));
			}

		private:
			wxFont m_Font;

		public:
			Font() = default;
			Font(const wxFont& other)
				:m_Font(other)
			{
			}
			Font(const wxNativeFontInfo& other)
				:m_Font(other)
			{
			}
			Font(int pointSize, FontFamily family, FlagSet<FontStyle> style, FontWeight weight, const String& faceName = {}, FontEncoding encoding = FontEncoding::Default)
				:m_Font(pointSize, static_cast<wxFontFamily>(family), Private::MapFontStyle(style), static_cast<wxFontWeight>(weight), style.Contains(FontStyle::Underline), faceName, static_cast<wxFontEncoding>(encoding))
			{
			}
			Font(const Size& pixelSize, FontFamily family, FlagSet<FontStyle> style, FontWeight weight, const String& faceName = {}, FontEncoding encoding = FontEncoding::Default)
				:m_Font(pixelSize, static_cast<wxFontFamily>(family), Private::MapFontStyle(style), static_cast<wxFontWeight>(weight), style.Contains(FontStyle::Underline), faceName, static_cast<wxFontEncoding>(encoding))
			{
			}
			virtual ~Font() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Font.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto font = other.QueryInterface<Font>())
				{
					return m_Font == font->m_Font;
				}
				return false;
			}
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return std::make_unique<Font>(m_Font);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// Font
			const wxFont& ToWxFont() const noexcept
			{
				return m_Font;
			}
			wxFont& ToWxFont() noexcept
			{
				return m_Font;
			}

			String GetDescription() const
			{
				return m_Font.GetNativeFontInfoUserDesc();
			}
			Font GetBaseFont() const
			{
				return m_Font.GetBaseFont();
			}

			String Serialize() const
			{
				return m_Font.GetNativeFontInfoDesc();
			}
			bool Deserialize(const String& serializedData)
			{
				return m_Font.SetNativeFontInfo(serializedData);
			}

			String GetFaceName() const
			{
				return m_Font.GetFaceName();
			}
			bool SetFaceName(const String& faceName)
			{
				return m_Font.SetFaceName(faceName);
			}

			int GetPointSize() const
			{
				return m_Font.GetPointSize();
			}
			void SetPointSize(int pointSize)
			{
				m_Font.SetPointSize(pointSize);
			}

			double GetFractionalPointSize() const
			{
				return m_Font.GetFractionalPointSize();
			}
			void SetFractionalPointSize(double pointSize)
			{
				m_Font.SetFractionalPointSize(pointSize);
			}

			Size GetPixelSize() const
			{
				return m_Font.GetPixelSize();
			}
			void SetPixelSize(const Size& pixelSize)
			{
				m_Font.SetPixelSize(pixelSize);
			}

			void SetSymbolicSize(FontSymbolicSize size)
			{
				m_Font.SetSymbolicSize(static_cast<wxFontSymbolicSize>(size));
			}
			void SetSymbolicSizeRelativeTo(FontSymbolicSize size, int base)
			{
				m_Font.SetSymbolicSizeRelativeTo(static_cast<wxFontSymbolicSize>(size), base);
			}
			void ScaleSzie(double scale)
			{
				m_Font.SetFractionalPointSize(m_Font.GetFractionalPointSize() * scale);
			}

			FontEncoding GetEncoding() const
			{
				return static_cast<FontEncoding>(m_Font.GetEncoding());
			}
			void SetEncoding(FontEncoding encoding)
			{
				m_Font.SetEncoding(static_cast<wxFontEncoding>(encoding));
			}

			FontFamily GetFamily() const
			{
				return static_cast<FontFamily>(m_Font.GetFamily());
			}
			void SetFamily(FontFamily family)
			{
				m_Font.SetFamily(static_cast<wxFontFamily>(family));
			}

			FlagSet<FontStyle> GetStyle() const
			{
				auto style = Private::MapFontStyle(m_Font.GetStyle());
				style.Add(FontStyle::Underline, m_Font.GetUnderlined());
				style.Add(FontStyle::Strikethrough, m_Font.GetStrikethrough());

				return style;
			}
			void SetStyle(FlagSet<FontStyle> style)
			{
				m_Font.SetStyle(Private::MapFontStyle(style));
				m_Font.SetUnderlined(style.Contains(FontStyle::Underline));
				m_Font.SetStrikethrough(style.Contains(FontStyle::Strikethrough));
			}
			void AddStyle(FlagSet<FontStyle> style)
			{
				SetStyle(GetStyle().Add(style));
			}
			void RemoveStyle(FlagSet<FontStyle> style)
			{
				SetStyle(GetStyle().Remove(style));
			}

			FontWeight GetWeight() const
			{
				return static_cast<FontWeight>(m_Font.GetWeight());
			}
			int GetNumericWeight() const
			{
				return m_Font.GetNumericWeight();
			}
			void SetWeight(FontWeight weight)
			{
				m_Font.SetWeight(static_cast<wxFontWeight>(weight));
			}
			void SetNumericWeight(int weight)
			{
				m_Font.SetNumericWeight(weight);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			Font& operator=(const Font& other)
			{
				m_Font = other.m_Font;

				return *this;
			}
	};
}
