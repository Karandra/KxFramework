#pragma once
#include "Common.h"
#include <KxFramework/KxOptionSet.h>

namespace KxDataView2
{
	class KX_API Renderer;
	class KX_API RenderEngine;
}

namespace KxDataView2::CellAttributeOptions::Enums
{
	enum class Option
	{
		None = 0,
		Enabled = 1 << 0,
		Editable = 1 << 1,
		ShowAccelerators = 1 << 2,

		Default = Enabled|Editable
	};
	enum class FontOption
	{
		None = 0,
		Bold = 1 << 0,
		Italic = 1 << 1,
		Underlined = 1 << 2,
		Strikethrough = 1 << 3,

		Default = None
	};
	enum class BGOption
	{
		None = 0,
		Header = 1 << 0,
		Button = 1 << 1,
		ComboBox = 1 << 2,

		Default = None
	};
}

namespace KxDataView2::CellAttributeOptions
{
	class Options final: public KxOptionSet<Enums::Option, Enums::Option::Default>
	{
		friend class Renderer;
		friend class RenderEngine;

		private:
			KxColor m_ForegroundColor;
			KxColor m_BackgroundColor;
			wxAlignment m_Alignment = wxALIGN_INVALID;

		private:
			bool NeedDCAlteration() const
			{
				return HasColors();
			}
			void ChangeAlpha(KxColor& color, uint8_t alpha) const
			{
				color.SetA(alpha);
			}

		public:
			bool IsDefault() const
			{
				return !HasColors() && !HasAlignment() && GetValue() == Enums::Option::Default;
			}

			// Colors
			bool HasColors() const
			{
				return HasBackgroundColor() || HasForegroundColor();
			}

			bool HasBackgroundColor() const
			{
				return m_BackgroundColor.IsOk();
			}
			KxColor GetBackgroundColor() const
			{
				return m_BackgroundColor;
			}
			void SetBackgroundColor(const KxColor& color)
			{
				m_BackgroundColor = color;
			}
			void SetBackgroundColor(const KxColor& color, uint8_t alpha)
			{
				m_BackgroundColor = color;
				ChangeAlpha(m_BackgroundColor, alpha);
			}

			bool HasForegroundColor() const
			{
				return m_ForegroundColor.IsOk();
			}
			KxColor GetForegroundColor() const
			{
				return m_ForegroundColor;
			}
			void SetForegroundColor(const KxColor& color)
			{
				m_ForegroundColor = color;
			}
			void SetForegroundColor(const KxColor& color, uint8_t alpha)
			{
				m_ForegroundColor = color;
				ChangeAlpha(m_ForegroundColor, alpha);
			}
			
			// Alignment
			bool HasAlignment() const
			{
				return m_Alignment != wxALIGN_INVALID;
			}
			wxAlignment GetAlignment() const
			{
				return m_Alignment;
			}
			void SetAlignment(wxAlignment alignment)
			{
				m_Alignment = alignment;
			}
			void SetAlignment(int alignment)
			{
				m_Alignment = static_cast<wxAlignment>(alignment);
			}
	};
}

namespace KxDataView2::CellAttributeOptions
{
	class FontOptions final: public KxOptionSet<Enums::FontOption, Enums::FontOption::Default>
	{
		friend class Renderer;
		friend class RenderEngine;

		private:
			wxString m_FontFace;
			int32_t m_FontSize = 0;

		private:
			bool NeedDCAlteration() const
			{
				return HasFontFace() || HasFontSize() || GetValue() != Enums::FontOption::Default;
			}

		public:
			bool IsDefault() const
			{
				return !NeedDCAlteration();
			}

			bool HasFontFace() const
			{
				return !m_FontFace.IsEmpty();
			}
			const wxString& GetFontFace() const
			{
				return m_FontFace;
			}
			void SetFontFace(const wxString& faceName)
			{
				m_FontFace = faceName;
			}

			bool HasFontSize() const
			{
				return m_FontSize > 0;
			}
			int32_t GetFontSize() const
			{
				return m_FontSize;
			}
			void SetFontSize(int32_t value)
			{
				m_FontSize = std::clamp(value, 0, std::numeric_limits<int32_t>::max());
			}
	};
}

namespace KxDataView2::CellAttributeOptions
{
	class BGOptions final: public KxOptionSet<Enums::BGOption, Enums::BGOption::Default>
	{
		friend class Renderer;
		friend class RenderEngine;

		private:
			bool NeedDCAlteration() const
			{
				return GetValue() != Enums::BGOption::Default;
			}

		public:
			bool IsDefault() const
			{
				return !NeedDCAlteration();
			}
	};
}
