#include "KxStdAfx.h"
#include "KxFramework/KxTopLevelWindow.h"
#include "KxFramework/KxColor.h"
#include "KxFramework/KxSystemAPI.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxGraphicsContext.h"
#include "KxFramework/KxIncludeWindows.h"
#include <DWMAPI.h>

bool KxTLWInternal::DWMIsCompositionEnabled()
{
	BOOL ret = FALSE;
	if (KxSystemAPI::DwmIsCompositionEnabled)
	{
		KxSystemAPI::DwmIsCompositionEnabled(&ret);
	}
	return ret;
}
bool KxTLWInternal::DWMIsGlassOpaque()
{
	DWORD nDwmColor = 0;
	BOOL bOpaqueColor = FALSE;
	if (KxSystemAPI::DwmGetColorizationColor)
	{
		KxSystemAPI::DwmGetColorizationColor(&nDwmColor, &bOpaqueColor);
	}
	return bOpaqueColor;
}
KxColor KxTLWInternal::DWMGetGlassColor()
{
	KxColor color;
	if (KxSystemAPI::DwmGetColorizationColor)
	{
		DWORD nDwmColor = 0;
		BOOL bOpaqueColor = FALSE;
		KxSystemAPI::DwmGetColorizationColor(&nDwmColor, &bOpaqueColor);

		color.SetARGB(nDwmColor);
	}
	return color;
}

void KxTLWInternal::SetDefaultBackgroundColor(wxWindow* window)
{
	// White: wxSYS_COLOUR_WINDOW, Gray: wxSYS_COLOUR_FRAMEBK
	window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
}
bool KxTLWInternal::MSWWindowProc(wxWindow* window, WXLRESULT& result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
{
	switch (msg)
	{
		case WM_DWMCOLORIZATIONCOLORCHANGED:
		{
			wxCommandEvent event(KxEVT_DWM_GLASS_COLOR_CHANGED);
			event.SetEventObject(window);
			window->HandleWindowEvent(event);

			result = 0;
			return true;
		}
		case WM_DWMCOMPOSITIONCHANGED:
		{
			wxCommandEvent event(KxEVT_DWM_COMPOSITION_CHANGED);
			event.SetEventObject(window);
			event.SetInt(DWMIsCompositionEnabled());
			window->HandleWindowEvent(event);

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
				KxMenuItem* item = NULL;
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

				if (flags & MF_POPUP && item)
				{
					
				}

				result = 0;
			}
			result = 1;
			return true;
		}
	};

	return false;
}

KxColor KxTLWInternal::DWMGetColorKey(const wxWindow* window)
{
	wxWindowDC dc(const_cast<wxWindow*>(window));
	wxImage image = dc.GetAsBitmap().ConvertToImage();

	wxColour color("PURPLE");
	wxByte R, G, B = 0;
	if (image.FindFirstUnusedColour(&R, &G, &B))
	{
		color.Set(R, G, B);
	}
	return color;
}
bool KxTLWInternal::DWMExtendFrame(wxWindow* window)
{
	if (KxSystemAPI::DwmExtendFrameIntoClientArea)
	{
		HWND hWnd = window->GetHandle();
		KxUtility::ToggleWindowStyle(hWnd, GWL_EXSTYLE, WS_EX_LAYERED, false);

		MARGINS margins = {0};
		return KxSystemAPI::DwmExtendFrameIntoClientArea(hWnd, &margins) == S_OK;
	}
	return false;
}
bool KxTLWInternal::DWMExtendFrame(wxWindow* window, const wxRect& rect, const wxColour& color)
{
	if (KxSystemAPI::DwmExtendFrameIntoClientArea)
	{
		HWND hWnd = window->GetHandle();
		KxUtility::ToggleWindowStyle(hWnd, GWL_EXSTYLE, WS_EX_LAYERED, true);

		if (color.IsOk())
		{
			SetLayeredWindowAttributes(hWnd, color.GetPixel(), color.Alpha(), LWA_COLORKEY);
		}
		else
		{
			SetLayeredWindowAttributes(hWnd, color.GetPixel(), 0, 0);
		}

		MARGINS margins = {0};
		margins.cxLeftWidth = rect.GetX();
		margins.cxRightWidth = rect.GetY();
		margins.cyTopHeight = rect.GetWidth();
		margins.cyBottomHeight = rect.GetHeight();
		return KxSystemAPI::DwmExtendFrameIntoClientArea(hWnd, &margins) == S_OK;
	}
	return false;
}
bool KxTLWInternal::DWMBlurBehind(wxWindow* window, bool enable, const wxRegion& region)
{
	if (KxSystemAPI::DwmEnableBlurBehindWindow)
	{
		DWM_BLURBEHIND tBlurInfo = {0};
		tBlurInfo.fEnable = enable;
		tBlurInfo.fTransitionOnMaximized = TRUE;
		tBlurInfo.dwFlags = DWM_BB_ENABLE|DWM_BB_TRANSITIONONMAXIMIZED;
		if (!region.IsEmpty())
		{
			tBlurInfo.dwFlags |= DWM_BB_BLURREGION;
			tBlurInfo.hRgnBlur = region.GetHRGN();
		}

		return KxSystemAPI::DwmEnableBlurBehindWindow(window->GetHandle(), &tBlurInfo);
	}
	return false;
}

wxIcon KxTLWInternal::GetTitleIcon(const wxWindow* window)
{
	HWND hWnd = window->GetHandle();
	HICON iconHandle = (HICON)::SendMessageW(hWnd, WM_GETICON, ICON_BIG, NULL);
	if (iconHandle == NULL)
	{
		iconHandle = (HICON)::SendMessageW(hWnd, WM_GETICON, ICON_SMALL, NULL);
		if (iconHandle != NULL)
		{
			wxIcon icon;
			icon.CreateFromHICON(iconHandle);
			return icon;
		}
	}
	return wxNullIcon;
}
void KxTLWInternal::SetTitleIcon(wxWindow* window, const wxIcon& icon)
{
	HWND hWnd = window->GetHandle();
	if (icon.IsOk())
	{
		::SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)icon.GetHICON());
		::SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon.GetHICON());
	}
	else
	{
		::SendMessageW(hWnd, WM_SETICON, ICON_BIG, NULL);
		::SendMessageW(hWnd, WM_SETICON, ICON_SMALL, NULL);
	}
}

void* KxTLWInternal::GetWindowUserData(HWND hWnd)
{
	#ifdef _WIN64
	return reinterpret_cast<void*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	#else
	return reinterpret_cast<void*>(GetWindowLongW(hWnd, GWL_USERDATA));
	#endif
}
void KxTLWInternal::SetWindowUserData(HWND hWnd, const void* data)
{
	#ifdef _WIN64
	SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
	#else
	SetWindowLongW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(data));
	#endif
}

//////////////////////////////////////////////////////////////////////////
wxDEFINE_EVENT(KxEVT_DWM_GLASS_COLOR_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(KxEVT_DWM_COMPOSITION_CHANGED, wxCommandEvent);
