#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "Kx/UI/StdIcon.h"

namespace KxFramework::UI::DataView
{
	class View;
	class MainWindow;
	class Node;
	class Column;
}

namespace KxFramework::UI::DataView
{
	class KX_API ToolTip final
	{
		friend class MainWindow;

		public:
			using StdIcon = KxFramework::StdIcon;

		public:
			template<class... Args>
			static ToolTip CreateDefaultForRenderer(Args&&... arg)
			{
				ToolTip tooltip(std::forward<Args>(arg)...);
				tooltip.DisplayOnlyIfClipped();
				return tooltip;
			}

		private:
			wxString m_Caption;
			wxString m_Message;
			std::variant<wxBitmap, StdIcon> m_Icon;
			const Column* m_AnchorColumn = nullptr;

			const Column* m_ClipTestColumn = nullptr;
			bool m_DisplayOnlyIfClipped = false;

			bool m_AutoHide = true;

		private:
			const Column& SelectAnchorColumn(const Column& currentColumn) const;
			const Column& SelectClipTestColumn(const Column& currentColumn) const;

			wxPoint GetPopupPosition(const Node& node, const Column& column) const;
			wxPoint AdjustPopupPosition(const Node& node, const wxPoint& pos) const;
			wxString StripMarkupIfNeeded(const Node& node, const Column& column, const wxString& text) const;

			bool Show(const Node& node, const Column& column);

		public:
			ToolTip(const wxString& message = {}, StdIcon icon = StdIcon::None)
				:m_Message(message), m_Icon(icon)
			{
			}
			ToolTip(const wxString& caption, const wxString& message, StdIcon icon = StdIcon::None)
				:m_Caption(caption), m_Message(message), m_Icon(icon)
			{
			}
			ToolTip(const wxString& caption, const wxString& message, const wxBitmap& bitmap)
				:m_Caption(caption), m_Message(message), m_Icon(bitmap)
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

			bool HasAnyIcon() const
			{
				return !m_Icon.valueless_by_exception() && (GetIconID() != StdIcon::None || GetIconBitmap().IsOk());
			}
			StdIcon GetIconID() const
			{
				if (const auto& value = std::get_if<StdIcon>(&m_Icon))
				{
					return *value;
				}
				return StdIcon::None;
			}
			wxBitmap GetIconBitmap() const
			{
				if (const auto& value = std::get_if<wxBitmap>(&m_Icon))
				{
					return *value;
				}
				return wxNullBitmap;
			}
			void SetIcon(StdIcon icon)
			{
				m_Icon = icon;
			}
			void SetIcon(const wxBitmap& icon)
			{
				m_Icon = icon;
			}
			void SetIcon(const wxIcon& icon)
			{
				m_Icon = icon;
			}

			void SetAnchorColumn(const Column& column)
			{
				m_AnchorColumn = &column;
			}
			void ResetAnchorColumn()
			{
				m_AnchorColumn = nullptr;
			}
			const Column* GetAnchorColumn() const
			{
				return m_AnchorColumn;
			}

			bool ShouldAutoHide() const
			{
				return m_AutoHide;
			}
			void AutoHide(bool value = true)
			{
				m_AutoHide = value;
			}

			const Column* GetClipTestColumn() const
			{
				return m_ClipTestColumn;
			}
			bool ShouldDisplayOnlyIfClipped() const
			{
				return m_DisplayOnlyIfClipped;
			}
			void DisplayOnlyIfClipped(bool value = true)
			{
				m_DisplayOnlyIfClipped = value;
				if (!value)
				{
					m_ClipTestColumn = nullptr;
				}
			}
			void DisplayOnlyIfClipped(const Column& column)
			{
				m_DisplayOnlyIfClipped = true;
				m_ClipTestColumn = &column;
			}
	};
}
