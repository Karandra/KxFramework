#include "stdafx.h"
#include "TopLevelWindow.h"
#include "KxFramework/KxMenu.h"
#include "Kx/Drawing/Common.h"
#include "Kx/System/NativeAPI.h"
#include "Kx/Utility/System.h"
#include <DWMAPI.h>
#include "Kx/System/UndefWindows.h"
#undef DWMAPI

namespace KxFramework::UI::Private
{
	bool TopLevelWindowBase::DWMIsCompositionEnabled()
	{
		BOOL result = FALSE;
		if (NativeAPI::DWMAPI::DwmIsCompositionEnabled)
		{
			NativeAPI::DWMAPI::DwmIsCompositionEnabled(&result);
		}
		return result;
	}
	bool TopLevelWindowBase::DWMIsGlassOpaque()
	{
		DWORD dwmColor = 0;
		BOOL isOpaqueColor = FALSE;
		if (NativeAPI::DWMAPI::DwmGetColorizationColor)
		{
			NativeAPI::DWMAPI::DwmGetColorizationColor(&dwmColor, &isOpaqueColor);
		}
		return isOpaqueColor;
	}
	Color TopLevelWindowBase::DWMGetGlassColor()
	{
		Color color;
		if (NativeAPI::DWMAPI::DwmGetColorizationColor)
		{
			DWORD dwmColor = 0;
			BOOL isOpaqueColor = FALSE;
			NativeAPI::DWMAPI::DwmGetColorizationColor(&dwmColor, &isOpaqueColor);

			color.SetARGB(dwmColor);
		}
		return color;
	}

	Color TopLevelWindowBase::DWMGetColorKey() const
	{
		wxWindowDC dc(m_Window);
		wxImage image = Drawing::ToImage(dc.GetAsBitmap());

		PackedRGB<unsigned char> rgb;
		if (image.FindFirstUnusedColour(&rgb.Red, &rgb.Green, &rgb.Blue))
		{
			return Color::FromFixed8(rgb.Red, rgb.Green, rgb.Blue);
		}
		return {};
	}
	bool TopLevelWindowBase::DWMExtendFrame()
	{
		if (NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea)
		{
			HWND handle = m_Window->GetHandle();
			Utility::ModWindowStyle(handle, GWL_EXSTYLE, WS_EX_LAYERED, false);

			MARGINS margins = {};
			return NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea(handle, &margins) == S_OK;
		}
		return false;
	}
	bool TopLevelWindowBase::DWMExtendFrame(const wxRect& rect, const Color& color)
	{
		if (NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea)
		{
			HWND handle = m_Window->GetHandle();
			Utility::ModWindowStyle(handle, GWL_EXSTYLE, WS_EX_LAYERED, true);

			if (color)
			{
				::SetLayeredWindowAttributes(handle, color.GetCOLORREF(), color.GetFixed8().Alpha, LWA_COLORKEY);
			}
			else
			{
				::SetLayeredWindowAttributes(handle, color.GetCOLORREF(), 0, 0);
			}

			MARGINS margins = {};
			margins.cxLeftWidth = rect.GetX();
			margins.cxRightWidth = rect.GetY();
			margins.cyTopHeight = rect.GetWidth();
			margins.cyBottomHeight = rect.GetHeight();
			return NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea(handle, &margins) == S_OK;
		}
		return false;
	}
	bool TopLevelWindowBase::DWMBlurBehind(bool enable, const wxRegion& region)
	{
		if (NativeAPI::DWMAPI::DwmEnableBlurBehindWindow)
		{
			DWM_BLURBEHIND blurInfo = {};
			blurInfo.fEnable = enable;
			blurInfo.fTransitionOnMaximized = TRUE;
			blurInfo.dwFlags = DWM_BB_ENABLE|DWM_BB_TRANSITIONONMAXIMIZED;
			if (!region.IsEmpty())
			{
				blurInfo.dwFlags |= DWM_BB_BLURREGION;
				blurInfo.hRgnBlur = region.GetHRGN();
			}

			return NativeAPI::DWMAPI::DwmEnableBlurBehindWindow(m_Window->GetHandle(), &blurInfo);
		}
		return false;
	}

	void TopLevelWindowBase::SetDefaultBackgroundColor()
	{
		// White: wxSYS_COLOUR_WINDOW, Gray: wxSYS_COLOUR_FRAMEBK
		m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	}
	bool TopLevelWindowBase::MSWWindowProc(WXLRESULT& result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		HWND handle = m_Window->GetHandle();

		switch (msg)
		{
			case WM_NCCREATE:
			{
				if (NativeAPI::User32::EnableNonClientDpiScaling)
				{
					NativeAPI::User32::EnableNonClientDpiScaling(handle);
				}
				return false;
			}
			case WM_DWMCOLORIZATIONCOLORCHANGED:
			{
				wxNotifyEvent event(EvtDWMGlassColorChanged);
				event.SetEventObject(m_Window);
				m_Window->HandleWindowEvent(event);

				result = 0;
				return true;
			}
			case WM_DWMCOMPOSITIONCHANGED:
			{
				wxNotifyEvent event(EvtDWMCompositionChanged);
				event.SetEventObject(m_Window);
				event.SetInt(DWMIsCompositionEnabled());
				m_Window->HandleWindowEvent(event);

				result = 0;
				return true;
			}
			case WM_MENUSELECT:
			{
				int nItemID = LOWORD(wParam);
				int flags = HIWORD(wParam);

				KxMenu* menu = KxMenu::GetCurrentMenu();
				if (menu && flags & MF_MOUSESELECT)
				{
					KxMenuItem* item = nullptr;
					if (flags & MF_POPUP)
					{
						item = menu->FindItemByPosition(nItemID);
					}

					if (item)
					{
						nItemID = item->GetId();

						// If this is popup menu container send sub-menu open event
						KxMenu* subMenu = item->GetSubMenu();
						if (flags & MF_POPUP && subMenu)
						{
							KxMenuEvent eventOpen(KxEVT_MENU_OPEN, subMenu, item);
							eventOpen.SetId(nItemID);
							eventOpen.SetEventObject(subMenu);
							subMenu->ProcessEvent(eventOpen);
						}
						else
						{
							KxMenuEvent event(KxEVT_MENU_HOVER, menu, item);
							event.SetId(nItemID);
							event.SetEventObject(item->GetEventHandler());
							if (!item->ProcessEvent(event))
							{
								// If item didn't processed this event send it to menu
								menu->ProcessEvent(event);
							}
						}
					}
					else
					{
						// KxMenu will dispatch this as needed
						wxMenuEvent event(wxEVT_MENU_HIGHLIGHT, nItemID, menu);
						event.SetId(nItemID);
						event.SetEventObject(menu);
						menu->ProcessEvent(event);
					}
				}
				result = 1;
				return true;
			}
		};

		return false;
	}

	wxIcon TopLevelWindowBase::GetTitleIcon() const
	{
		HWND handle = m_Window->GetHandle();
		HICON iconHandle = reinterpret_cast<HICON>(::SendMessageW(handle, WM_GETICON, ICON_BIG, 0));
		if (!iconHandle)
		{
			iconHandle = reinterpret_cast<HICON>(::SendMessageW(handle, WM_GETICON, ICON_SMALL, 0));
			if (iconHandle)
			{
				wxIcon icon;
				icon.CreateFromHICON(iconHandle);
				return icon;
			}
		}
		return wxNullIcon;
	}
	void TopLevelWindowBase::SetTitleIcon(const wxIcon& icon)
	{
		HWND handle = m_Window->GetHandle();
		if (icon.IsOk())
		{
			::SendMessageW(handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon.GetHICON()));
			::SendMessageW(handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon.GetHICON()));
		}
		else
		{
			::SendMessageW(handle, WM_SETICON, ICON_BIG, 0);
			::SendMessageW(handle, WM_SETICON, ICON_SMALL, 0);
		}
	}

	bool TopLevelWindowBase::EnableCloseButton(bool enable)
	{
		HWND handle = m_Window->GetHandle();

		UINT options = 0;
		if (enable)
		{
			options = MF_ENABLED;
		}
		else
		{
			options = MF_DISABLED|MF_GRAYED;
		}
		return ::EnableMenuItem(::GetSystemMenu(handle, FALSE), SC_CLOSE, MF_BYCOMMAND|options);
	}
	bool TopLevelWindowBase::EnableMinimizeButton(bool enable)
	{
		HWND handle = m_Window->GetHandle();
		Utility::ModWindowStyle(handle, GWL_STYLE, WS_MINIMIZEBOX, enable);
		return true;
	}
	bool TopLevelWindowBase::EnableMaximizeButton(bool enable)
	{
		HWND handle = m_Window->GetHandle();
		Utility::ModWindowStyle(handle, GWL_STYLE, WS_MAXIMIZEBOX, enable);
		return true;
	}

	void* TopLevelWindowBase::GetWindowUserData() const
	{
		HWND handle = m_Window->GetHandle();
		#ifdef _WIN64
		return reinterpret_cast<void*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
		#else
		return reinterpret_cast<void*>(GetWindowLongW(handle, GWL_USERDATA));
		#endif
	}
	void TopLevelWindowBase::SetWindowUserData(const void* data)
	{
		HWND handle = m_Window->GetHandle();
		#ifdef _WIN64
		SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
		#else
		SetWindowLongW(handle, GWLP_USERDATA, reinterpret_cast<LONG>(data));
		#endif
	}
}
