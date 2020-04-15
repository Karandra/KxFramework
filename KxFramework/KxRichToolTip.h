#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/UI/StdIcon.h"

class KX_API KxRichToolTip: public wxObject
{
	public:
		using StdIcon = KxFramework::StdIcon;

	private:
		wxRichToolTip m_ToolTip;

		wxString m_Title;
		wxString m_Message;
		wxIcon m_Icon;
		wxFont m_Font;
		StdIcon m_IconID = DefaultIcon;
		wxTipKind m_Kind = DefaultKind;
		wxColour m_Color1;
		wxColour m_Color2;
		int m_Timeout = -1;
		int m_Delay = -1;

	public:
		static const StdIcon DefaultIcon = StdIcon::Information;
		static const wxTipKind DefaultKind = wxTipKind_Auto;
		static const int DefaultDelay = 0;

		KxRichToolTip(const wxString& title = {}, const wxString& message = {});

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
		
		void Show(wxWindow* window, const wxRect& rect = {})
		{
			m_ToolTip.ShowFor(window, rect.IsEmpty() ? nullptr : &rect);
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
		void GetBackgroundColour(wxColour* color1 = nullptr, wxColour* color2 = nullptr) const;

		// Icon
		StdIcon GetIconID() const
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
			m_IconID = StdIcon::None;
			m_ToolTip.SetIcon(icon);
		}
		void SetIcon(StdIcon iconID)
		{
			m_Icon = wxNullIcon;
			m_IconID = iconID;
			m_ToolTip.SetIcon(KxFramework::UI::ToWxStdIcon(iconID));
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
