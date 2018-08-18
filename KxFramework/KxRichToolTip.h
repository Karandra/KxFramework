#pragma once
#include "KxFramework/KxFramework.h"

class KxRichToolTip: public wxObject
{
	private:
		wxRichToolTip m_ToolTip;

		wxString m_Title;
		wxString m_Message;
		wxIcon m_Icon;
		wxFont m_Font;
		KxIconType m_IconID = DefaultIcon;
		wxTipKind m_Kind = DefaultKind;
		wxColour m_Color1;
		wxColour m_Color2;
		int m_Timeout = -1;
		int m_Delay = -1;

	public:
		static const KxIconType DefaultIcon = KxICON_INFO;
		static const wxTipKind DefaultKind = wxTipKind_Auto;
		static const int DefaultDelay = 0;

		KxRichToolTip(const wxString& title = wxEmptyString, const wxString& message = wxEmptyString);
		virtual ~KxRichToolTip();

	public:
		const wxString& GetTitle() const
		{
			return m_Title;
		}
		const wxString& GetMessage() const
		{
			return m_Message;
		}
		
		wxTipKind GetKind() const
		{
			return m_Kind;
		}
		void SetKind(wxTipKind kind)
		{
			m_Kind = kind;
			m_ToolTip.SetTipKind(kind);
		}
		
		void Show(wxWindow* window, const wxRect& rect = wxNullRect)
		{
			m_ToolTip.ShowFor(window, rect.IsEmpty() ? NULL : &rect);
		}

		// Appearance
		const wxFont& GetTitleFont() const
		{
			return m_Font;
		}
		void SetTitleFont(const wxFont& font)
		{
			m_Font = font;
			m_ToolTip.SetTitleFont(font);
		}
		void SetBackgroundColour(const wxColour& color1, const wxColour& color2 = wxNullColour)
		{
			m_Color1 = color1;
			m_Color2 = color2;
			m_ToolTip.SetBackgroundColour(color1, color2);
		}
		void GetBackgroundColour(wxColour* color1 = NULL, wxColour* color2 = NULL) const;

		// Icon
		KxIconType GetIconID() const
		{
			return m_IconID;
		}
		const wxIcon& GetIcon() const
		{
			if (m_Icon.IsOk())
			{
				return m_Icon;
			}
			return wxNullIcon;
		}
		void SetIcon(const wxIcon& icon)
		{
			m_Icon = icon;
			m_IconID = KxICON_NONE;
			m_ToolTip.SetIcon(icon);
		}
		void SetIcon(KxIconType iconID)
		{
			m_Icon = wxNullIcon;
			m_IconID = iconID;
			m_ToolTip.SetIcon(iconID);
		}

		// Animation
		int GetTimeout() const
		{
			return m_Timeout;
		}
		void SetTimeout(int timeout)
		{
			m_Timeout = timeout;
			m_ToolTip.SetTimeout(timeout, GetDelay());
		}
		
		int GetDelay() const
		{
			return m_Delay;
		}
		void SetDelay(int delay)
		{
			m_Delay = delay;
			m_ToolTip.SetTimeout(GetTimeout(), delay);
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxRichToolTip);
};
