#pragma once
#include "Kx/UI/Common.h"
#include "Kx/Utility/Common.h"
#include <wx/richtooltip.h>

namespace KxFramework::UI
{
	enum class RichToolTipKind
	{
		None = wxTipKind::wxTipKind_None,
		Auto = wxTipKind::wxTipKind_Auto,

		Top = wxTipKind::wxTipKind_Top,
		TopLeft = wxTipKind::wxTipKind_TopLeft,
		TopRight = wxTipKind::wxTipKind_TopRight,
		Bottom = wxTipKind::wxTipKind_Bottom,
		BottomLeft = wxTipKind::wxTipKind_BottomLeft,
		BottomRight = wxTipKind::wxTipKind_BottomRight
	};
}
namespace KxFramework::UI
{
	class KX_API RichToolTip: public wxObject
	{
		public:
			static constexpr StdIcon DefaultIcon = StdIcon::Information;
			static constexpr RichToolTipKind DefaultKind = RichToolTipKind::Auto;
			static inline const wxTimeSpan DefaultDelay = wxTimeSpan::Milliseconds(0);

		private:
			wxRichToolTip m_ToolTip;

			String m_Title;
			String m_Message;
			wxIcon m_Icon;
			wxFont m_Font;
			StdIcon m_IconID = DefaultIcon;
			RichToolTipKind m_Kind = DefaultKind;
			Color m_Color1;
			Color m_Color2;
			wxTimeSpan m_Timeout = wxTimeSpan::Milliseconds(-1);
			wxTimeSpan m_Delay = wxTimeSpan::Milliseconds(-1);

		public:
			RichToolTip(String title = {}, String message = {})
				:m_ToolTip(title, message), m_Title(std::move(title)), m_Message(std::move(message))
			{
			}

		public:
			void Show(wxWindow* window, const wxRect& rect = {})
			{
				m_ToolTip.ShowFor(window, rect.IsEmpty() ? nullptr : &rect);
			}
			
			String GetTitle() const
			{
				return m_Title;
			}
			String GetMessage() const
			{
				return m_Message;
			}
		
			RichToolTipKind GetKind() const
			{
				return m_Kind;
			}
			void SetKind(RichToolTipKind kind)
			{
				m_Kind = kind;
				m_ToolTip.SetTipKind(static_cast<wxTipKind>(kind));
			}

			// Appearance
			wxFont GetTitleFont() const
			{
				return m_Font;
			}
			void SetTitleFont(const wxFont& font)
			{
				m_Font = font;
				m_ToolTip.SetTitleFont(font);
			}
			void SetBackgroundColour(const Color& color1, const Color& color2 = wxNullColour)
			{
				m_Color1 = color1;
				m_Color2 = color2;
				m_ToolTip.SetBackgroundColour(color1, color2);
			}
			void GetBackgroundColour(Color* color1 = nullptr, Color* color2 = nullptr) const
			{
				Utility::SetIfNotNull(color1, m_Color1);
				Utility::SetIfNotNull(color2, m_Color2);
			}

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
			wxTimeSpan GetTimeout() const
			{
				return m_Timeout;
			}
			void SetTimeout(wxTimeSpan timeout)
			{
				m_Timeout = timeout;
				m_ToolTip.SetTimeout(timeout.GetMilliseconds().GetValue(), GetDelay().GetMilliseconds().GetValue());
			}
		
			wxTimeSpan GetDelay() const
			{
				return m_Delay;
			}
			void SetDelay(wxTimeSpan delay)
			{
				m_Delay = delay;
				m_ToolTip.SetTimeout(GetTimeout().GetMilliseconds().GetValue(), delay.GetMilliseconds().GetValue());
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(RichToolTip);
	};
}
