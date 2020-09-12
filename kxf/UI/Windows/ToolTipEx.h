#pragma once
#include "kxf/UI/Common.h"
#include "kxf/General/WithOptions.h"
#include <wx/nativewin.h>
#include <variant>
struct tagNMHDR;

namespace kxf::UI
{
	enum class ToolTipExStyle: uint32_t
	{
		None = 0,

		Ballon = 1 << 0,
		CloseButton = 1 << 1,
		LargeIcons = 1 << 2,
		AutoHide = 1 << 3,
	};
}
namespace kxf
{
	KxFlagSet_Declare(UI::ToolTipExStyle);
}

namespace kxf::UI
{
	class KX_API ToolTipEx: public wxNativeWindow, public WithOptions<ToolTipExStyle, ToolTipExStyle::None>
	{
		private:
			String m_Caption;
			String m_Message;
			std::variant<wxIcon, StdIcon> m_Icon = StdIcon::None;
			wxIcon m_QuestionIcon;
			int m_MaxWidth = -1;
			bool m_IsBalloonStyleSupported = false;

		private:
			bool CreateWindow(wxWindow* parent);
			bool CreateToolTip();
			void UpdateCaption();
			void UpdateStyle();
			int ConvertIconID(StdIcon icon) const;
			void DoRedraw();

		protected:
			bool MSWHandleMessage(WXLRESULT* result, WXUINT message, WXWPARAM wParam, WXLPARAM lParam) override;
			bool MSWHandleNotify(WXLRESULT* result, WXUINT message, WXWPARAM wParam, WXLPARAM lParam, const tagNMHDR& notificationInfo);

		public:
			ToolTipEx() = default;
			ToolTipEx(wxWindow* parent)
			{
				Create(parent);
			}
			bool Create(wxWindow* parent)
			{
				return CreateWindow(parent);
			}

		public:
			bool Popup(const Point& position, wxWindow* window = nullptr);
			void Dismiss();
			void Disown() = delete;

			String GetCaption() const
			{
				return m_Caption;
			}
			void SetCaption(const String& value)
			{
				m_Caption = value;
			}

			String GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(const String& value)
			{
				m_Message = value;
			}

			StdIcon GetIconID() const;
			wxIcon GetIcon() const;
			void SetIcon(const wxIcon& icon);
			void SetIcon(const wxBitmap& bitmap)
			{
				wxIcon icon;
				icon.CopyFromBitmap(bitmap);
				SetIcon(icon);
			}
			void SetIcon(StdIcon iconID);

			int GetMaxWidth() const
			{
				return m_MaxWidth;
			}
			void SetMaxWidth(int maxWidth)
			{
				m_MaxWidth = maxWidth;
			}
	};
}
