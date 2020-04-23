#include "stdafx.h"
#include "ToolTipEx.h"
#include "Kx/System/Registry.h"
#include <CommCtrl.h>

namespace
{
	constexpr uint32_t g_DefaultStyle = WS_POPUP;
	constexpr uint32_t g_DefaultExStyle = WS_EX_TOOLWINDOW;

	bool IsBalloonStyleSupported()
	{
		using namespace KxFramework;

		RegistryKey key(RegistryBaseKey::CurrentUser, wxS("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"), RegistryAccess::Read);
		if (key)
		{
			return key.GetUInt32Value(wxS("EnableBalloonTips")) != 0;
		}
		return true;
	}
	TTTOOLINFOW MakeToolInfo(const KxFramework::UI::ToolTipEx& tooltip, const KxFramework::String& message)
	{
		TTTOOLINFOW info = {};
		info.cbSize = sizeof(info);
		info.hwnd = tooltip.GetParent()->GetHandle();
		info.lpszText = const_cast<wchar_t*>(message.wc_str());
		info.uFlags = TTF_TRACK|TTF_ABSOLUTE;

		return info;
	}
}

namespace KxFramework::UI
{
	bool ToolTipEx::CreateWindow(wxWindow* parent)
	{
		m_IsBalloonStyleSupported = IsBalloonStyleSupported();

		if (parent)
		{
			HWND handle = ::CreateWindowExW(g_DefaultExStyle,
											TOOLTIPS_CLASS,
											nullptr,
											g_DefaultStyle,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											parent->GetHandle(),
											nullptr,
											nullptr,
											nullptr
			);

			if (handle)
			{
				if (wxNativeWindow::Create(parent, wxID_NONE, handle))
				{
					wxNativeWindow::Disown();
					return true;
				}
				else
				{
					::DestroyWindow(handle);
				}
			}
		}
		return false;
	}
	bool ToolTipEx::CreateToolTip()
	{
		if (GetParent())
		{
			HWND hwnd = GetHandle();

			// Set basic info
			UpdateStyle();

			TTTOOLINFOW info = MakeToolInfo(*this, m_Message);
			::SendMessageW(hwnd, TTM_DELTOOL, 0, reinterpret_cast<LPARAM>(&info));
			::SendMessageW(hwnd, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&info));

			// Set max width
			::SendMessageW(hwnd, TTM_SETMAXTIPWIDTH, 0, m_MaxWidth);

			// Set timeout
			::SendMessageW(hwnd, TTM_SETDELAYTIME, TTDT_AUTOMATIC, MAKELONG(-1, 0));

			// Set icon and caption
			UpdateCaption();

			return true;
		}
		return false;
	}
	void ToolTipEx::UpdateCaption()
	{
		HWND hwnd = GetHandle();

		auto DoUpdateCaption = [this, hwnd]()
		{
			if (const wxIcon* icon = std::get_if<wxIcon>(&m_Icon); icon && icon->IsOk())
			{
				::SendMessageW(hwnd, TTM_SETTITLE, reinterpret_cast<WPARAM>(icon->GetHandle()), reinterpret_cast<LPARAM>(m_Caption.wc_str()));
				return true;
			}
			else if (const StdIcon* iconType = std::get_if<StdIcon>(&m_Icon))
			{
				// There's no corresponding question icon in tooltip icons enum, so get it ourselves
				if (*iconType == StdIcon::Question)
				{
					if (IsOptionEnabled(ToolTipExStyle::LargeIcons))
					{
						m_QuestionIcon = wxArtProvider::GetMessageBoxIcon(wxICON_QUESTION);
					}
					else
					{
						const int width = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
						const int height = wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y);
						m_QuestionIcon = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX, wxSize(width, height));
					}

					if (m_QuestionIcon.IsOk())
					{
						::SendMessageW(hwnd, TTM_SETTITLE, reinterpret_cast<WPARAM>(m_QuestionIcon.GetHandle()), reinterpret_cast<LPARAM>(m_Caption.wc_str()));
						return true;
					}
				}
				else
				{
					::SendMessageW(hwnd, TTM_SETTITLE, static_cast<WPARAM>(ConvertIconID(*iconType)), reinterpret_cast<LPARAM>(m_Caption.wc_str()));
					return true;
				}
			}
			return false;
		};
		if (!DoUpdateCaption())
		{
			// Set just the text caption, without icon
			::SendMessageW(hwnd, TTM_SETTITLE, 0, reinterpret_cast<LPARAM>(m_Caption.wc_str()));
		}
	}
	void ToolTipEx::UpdateStyle()
	{
		uint32_t style = g_DefaultStyle|TTS_ALWAYSTIP|TTS_NOPREFIX;
		if (IsOptionEnabled(ToolTipExStyle::Ballon) && m_IsBalloonStyleSupported)
		{
			style |= TTS_BALLOON;
		}
		if (IsOptionEnabled(ToolTipExStyle::CloseButton))
		{
			style |= TTS_CLOSE;
		}

		::SetWindowLongPtrW(GetHandle(), GWL_STYLE, style);
	}
	int ToolTipEx::ConvertIconID(StdIcon icon) const
	{
		const bool largeIcons = IsOptionEnabled(ToolTipExStyle::LargeIcons);
		switch (icon)
		{
			case StdIcon::Information:
			{
				return largeIcons ? TTI_INFO_LARGE : TTI_INFO;
			}
			case StdIcon::Warning:
			{
				return largeIcons ? TTI_WARNING_LARGE : TTI_WARNING;
			}
			case StdIcon::Error:
			{
				return largeIcons ? TTI_ERROR_LARGE : TTI_ERROR;
			}
		};
		return TTI_NONE;
	}
	void ToolTipEx::DoRedraw()
	{
		::SendMessageW(GetHandle(), TTM_UPDATE, 0, 0);
	}

	bool ToolTipEx::MSWHandleMessage(WXLRESULT* result, WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
	{
		switch (message)
		{
			case WM_NOTIFY:
			{
				const NMHDR* notifyInfo = reinterpret_cast<const NMHDR*>(lParam);
				if (notifyInfo && MSWHandleNotify(result, message, wParam, lParam, *notifyInfo))
				{
					return true;
				}
				break;
			}
		};
		return wxNativeWindow::MSWHandleMessage(result, message, wParam, lParam);
	}
	bool ToolTipEx::MSWHandleNotify(WXLRESULT* result, WXUINT message, WXWPARAM wParam, WXLPARAM lParam, const tagNMHDR& notifyInfo)
	{
		switch (notifyInfo.code)
		{
			case TTN_SHOW:
			{
				wxShowEvent event(GetId(), true);
				event.SetEventObject(this);
				HandleWindowEvent(event);
				return false;
			}
			case TTN_POP:
			{
				wxShowEvent event(GetId(), false);
				event.SetEventObject(this);
				HandleWindowEvent(event);
				return false;
			}
			case TTN_LINKCLICK:
			{
				wxTextUrlEvent event(GetId(), wxMouseEvent(wxEVT_LEFT_UP), 0, 0);
				event.SetEventObject(this);
				HandleWindowEvent(event);
				return true;
			}
			case TTN_GETDISPINFO:
			{
				NMTTDISPINFOW* displayInfo = reinterpret_cast<NMTTDISPINFOW*>(lParam);
				if (displayInfo)
				{
					displayInfo->lpszText = const_cast<wchar_t*>(m_Message.wc_str());
					return true;
				}
				return false;
			}
		};
		return false;
	}

	bool ToolTipEx::Popup(const wxPoint& position, wxWindow* window)
	{
		if (CreateToolTip())
		{
			Dismiss();

			if (HWND hwnd = GetHandle())
			{
				wxPoint pos = (window ? window : GetParent())->ClientToScreen(position);
				::SendMessageW(hwnd, TTM_TRACKPOSITION, 0, static_cast<LPARAM>(MAKELONG(pos.x, pos.y)));

				TTTOOLINFOW info = MakeToolInfo(*this, m_Message);
				::SendMessageW(hwnd, TTM_TRACKACTIVATE, TRUE, reinterpret_cast<LPARAM>(&info));

				return true;
			}
		}
		return false;
	}
	void ToolTipEx::Dismiss()
	{
		TTTOOLINFOW info = MakeToolInfo(*this, m_Message);
		::SendMessageW(GetHandle(), TTM_TRACKACTIVATE, FALSE, reinterpret_cast<LPARAM>(&info));
	}

	StdIcon ToolTipEx::GetIconID() const
	{
		if (const StdIcon* iconType = std::get_if<StdIcon>(&m_Icon))
		{
			return *iconType;
		}
		return StdIcon::None;
	}
	wxIcon ToolTipEx::GetIcon() const
	{
		if (const wxIcon* icon = std::get_if<wxIcon>(&m_Icon))
		{
			return *icon;
		}
		return wxNullIcon;
	}
	void ToolTipEx::SetIcon(const wxIcon& icon)
	{
		m_Icon = icon;
	}
	void ToolTipEx::SetIcon(StdIcon iconID)
	{
		switch (iconID)
		{
			case StdIcon::Error:
			case StdIcon::Warning:
			case StdIcon::Question:
			case StdIcon::Information:
			{
				m_Icon = iconID;
				break;
			}
			default:
			{
				m_Icon = StdIcon::None;
				break;
			}
		};
	}
}
