#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"

class KX_API KxDataViewItemAttributes
{
	private:
		// Colors
		KxColor m_BackgroundColor;
		KxColor m_ForegroundColor;

		// Font
		wxString m_FontFace;
		wxFontFamily m_FontFamily = wxFONTFAMILY_DEFAULT;
		int m_FontSize = 0;
		wxSize m_FontPixelSize = wxDefaultSize;
		bool m_IsBold = false;
		bool m_IsItalic = false;
		bool m_IsUnderlined = false;
		bool m_IsStrikethrough = false;

		// Misc
		bool m_IsEnabled = true;
		bool m_ButtonBackgound = false;
		bool m_HeaderButtonBackgound = false;
		bool m_ShowAccelerators = false;
		wxAlignment m_Alignment = wxALIGN_INVALID;

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
			return m_BackgroundColor.IsValid();
		}
		const KxColor& GetBackgroundColor() const
		{
			return m_BackgroundColor;
		}
		void SetBackgroundColor(const KxColor& color)
		{
			m_BackgroundColor = color;
		}

		bool HasForegroundColor() const
		{
			return m_ForegroundColor.IsValid();
		}
		const KxColor& GetForegroundColor() const
		{
			return m_ForegroundColor;
		}
		void SetForegroundColor(const KxColor& color)
		{
			m_ForegroundColor = color;
		}

		/* Font attributes */
		bool HasFontAttributes() const
		{
			return HasFontFace() || HasFontFamily() || HasFontSize() || HasFontPixelSize() || IsBold() || IsItalic() || IsUnderlined() || IsStrikethrough();
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

		bool HasFontFamily() const
		{
			return m_FontFamily != wxFONTFAMILY_DEFAULT && m_FontFamily < wxFONTFAMILY_MAX;
		}
		wxFontFamily GetFontFamily() const
		{
			return m_FontFamily;
		}
		void SetFontFamily(wxFontFamily family)
		{
			m_FontFamily = family;
		}

		bool HasFontPixelSize() const
		{
			return m_FontPixelSize.IsFullySpecified();
		}
		wxSize GetFontPixelSize() const
		{
			return m_FontPixelSize;
		}
		void SetFontPixelSize(const wxSize& size)
		{
			m_FontPixelSize = size;
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
			return HasHeaderButtonBackgound() || HasButtonBackgound() || ShouldShowAccelerators() || HasAlignment();
		}
		
		bool IsEnabled() const
		{
			return m_IsEnabled;
		}
		void SetEnabled(bool enabled = true)
		{
			m_IsEnabled = enabled;
		}

		bool HasHeaderButtonBackgound() const
		{
			return m_HeaderButtonBackgound;
		}
		void SetHeaderButtonBackgound(bool enabled = true)
		{
			m_HeaderButtonBackgound = enabled;
		}
		
		bool HasButtonBackgound() const
		{
			return m_ButtonBackgound;
		}
		void SetButtonBackgound(bool enabled = true)
		{
			m_ButtonBackgound = enabled;
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
