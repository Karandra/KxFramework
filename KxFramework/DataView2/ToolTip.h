#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"

namespace KxDataView2
{
	class KX_API View;
	class KX_API MainWindow;
	class KX_API Node;
	class KX_API Column;
}

namespace KxDataView2
{
	class KX_API ToolTip
	{
		friend class MainWindow;

		public:
			template<class... Args> static ToolTip CreateDefaultForRenderer(Args&&... arg)
			{
				ToolTip tooltip(std::forward<Args>(arg)...);
				tooltip.DisplayOnlyIfClipped();
				return tooltip;
			}

		private:
			wxString m_Caption;
			wxString m_Message;
			KxIconType m_Icon = KxICON_NONE;
			bool m_AutoHide = true;
			bool m_DisplayOnlyIfClipped = false;

		private:
			bool Show(const Node& node, const Column& column);

		public:
			ToolTip() = default;
			ToolTip(const wxString& message, KxIconType icon = KxICON_NONE)
				:m_Message(message), m_Icon(icon)
			{
			}
			ToolTip(const wxString& caption, const wxString& message, KxIconType icon = KxICON_NONE)
				:m_Caption(caption), m_Message(message), m_Icon(icon)
			{
			}

		public:
			bool IsOK() const;
			
			wxString GetCaption() const
			{
				return m_Caption;
			}
			void SetCaption(const wxString& value)
			{
				m_Caption = value;
			}

			wxString GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(const wxString& value)
			{
				m_Message = value;
			}

			KxIconType GetIcon() const
			{
				return m_Icon;
			}
			void SetIcon(KxIconType icon)
			{
				m_Icon = icon;
			}

			bool ShouldAutoHide() const
			{
				return m_AutoHide;
			}
			void AutoHide(bool value = true)
			{
				m_AutoHide = value;
			}

			bool ShouldDisplayOnlyIfClipped() const
			{
				return m_DisplayOnlyIfClipped;
			}
			void DisplayOnlyIfClipped(bool value = true)
			{
				m_DisplayOnlyIfClipped = value;
			}
	};
}
