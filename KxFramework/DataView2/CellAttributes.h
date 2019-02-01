#pragma once
#include "Common.h"

namespace KxDataView2
{
	class KX_API CellAttributes
	{
		private:
			// Colors
			KxColor m_BackgroundColor;
			KxColor m_ForegroundColor;

			// Font
			wxString m_FontFace;
			int m_FontSize = 0;
			bool m_IsBold = false;
			bool m_IsItalic = false;
			bool m_IsUnderlined = false;
			bool m_IsStrikethrough = false;

			// Misc
			bool m_IsEnabled = true;
			bool m_HeaderBackgound = false;
			bool m_ShowAccelerators = false;
			wxAlignment m_Alignment = wxALIGN_INVALID;

		private:
			void ChangeAlpha(KxColor& color, uint8_t alpha)
			{
				color.SetA(alpha);
			}

		public:
			/* General */
			bool IsDefault() const
			{
				return !HasColors() && !HasFontAttributes() && !HasItemState() && !HasMiscAttributes();
			}
			void Reset();
			wxFont GetEffectiveFont(const wxFont& baseFont) const;

			/* Colors */
			bool HasColors() const
			{
				return HasBackgroundColor() || HasForegroundColor();
			}

			bool HasBackgroundColor() const
			{
				return m_BackgroundColor.IsOk();
			}
			const KxColor& GetBackgroundColor() const
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
			const KxColor& GetForegroundColor() const
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

			/* Font attributes */
			bool HasFontAttributes() const
			{
				return HasFontFace() || HasFontSize() || IsBold() || IsItalic() || IsUnderlined() || IsStrikethrough();
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
			int GetFontSize() const
			{
				return m_FontSize;
			}
			void SetFontSize(int value)
			{
				m_FontSize = value;
			}

			bool IsBold() const
			{
				return m_IsBold;
			}
			void SetBold(bool value = true)
			{
				m_IsBold = value;
			}

			bool IsItalic() const
			{
				return m_IsItalic;
			}
			void SetItalic(bool value = true)
			{
				m_IsItalic = value;
			}

			bool IsUnderlined() const
			{
				return m_IsUnderlined;
			}
			void SetUnderlined(bool value = true)
			{
				m_IsUnderlined = value;
			}

			bool IsStrikethrough() const
			{
				return m_IsStrikethrough;
			}
			void SetStrikethrough(bool value = true)
			{
				m_IsStrikethrough = value;
			}

			/* Misc attributes */
			bool HasItemState() const
			{
				return m_IsEnabled != true;
			}
			bool HasMiscAttributes() const
			{
				return HasHeaderBackgound() || ShouldShowAccelerators() || HasAlignment();
			}
		
			bool IsEnabled() const
			{
				return m_IsEnabled;
			}
			void SetEnabled(bool enabled = true)
			{
				m_IsEnabled = enabled;
			}

			bool HasHeaderBackgound() const
			{
				return m_HeaderBackgound;
			}
			void SetHeaderBackgound(bool enabled = true)
			{
				m_HeaderBackgound = enabled;
			}
		
			bool ShouldShowAccelerators() const
			{
				return m_ShowAccelerators;
			}
			void ShowAccelerators(bool enabled = true)
			{
				m_ShowAccelerators = enabled;
			}

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
