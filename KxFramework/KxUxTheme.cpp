#include "KxStdAfx.h"
#include "KxFramework/KxUxTheme.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxDCUtility.h"
#include "Kx/Utility/Common.h"
#include <wx/fontutil.h>

#include <Uxtheme.h>
#include "KxUxThemePartsAndStates.h"
using namespace KxEnumClassOperations;

namespace
{
	const wchar_t* MapKxUxThemeClassToName(KxUxThemeClass KxUxThemeClass)
	{
		switch (KxUxThemeClass)
		{
			case KxUxThemeClass::Button:
			{
				return L"BUTTON";
			}
			case KxUxThemeClass::Clock:
			{
				return L"CLOCK";
			}
			case KxUxThemeClass::ComboBox:
			{
				return L"COMBOBOX";
			}
			case KxUxThemeClass::Communications:
			{
				return L"COMMUNICATIONS";
			}
			case KxUxThemeClass::ControlPanel:
			{
				return L"CONTROLPANEL";
			}
			case KxUxThemeClass::DatePicker:
			{
				return L"DATEPICKER";
			}
			case KxUxThemeClass::DragDrop:
			{
				return L"DRAGDROP";
			}
			case KxUxThemeClass::Edit:
			{
				return L"EDIT";
			}
			case KxUxThemeClass::ExplorerBar:
			{
				return L"EXPLORERBAR";
			}
			case KxUxThemeClass::FlyOut:
			{
				return L"FLYOUT";
			}
			case KxUxThemeClass::Globals:
			{
				return L"GLOBALS";
			}
			case KxUxThemeClass::Header:
			{
				return L"HEADER";
			}
			case KxUxThemeClass::ListBox:
			{
				return L"LISTBOX";
			}
			case KxUxThemeClass::ListView:
			{
				return L"LISTVIEW";
			}
			case KxUxThemeClass::Menu:
			{
				return L"MENU";
			}
			case KxUxThemeClass::MenuBand:
			{
				return L"MENUBAND";
			}
			case KxUxThemeClass::Navigation:
			{
				return L"NAVIGATION";
			}
			case KxUxThemeClass::Page:
			{
				return L"PAGE";
			}
			case KxUxThemeClass::Progress:
			{
				return L"PROGRESS";
			}
			case KxUxThemeClass::Rebar:
			{
				return L"REBAR";
			}
			case KxUxThemeClass::ScrollBar:
			{
				return L"SCROLLBAR";
			}
			case KxUxThemeClass::SearchEditBox:
			{
				return L"SEARCHEDITBOX";
			}
			case KxUxThemeClass::Spin:
			{
				return L"SPIN";
			}
			case KxUxThemeClass::StartPanel:
			{
				return L"STARTPANEL";
			}
			case KxUxThemeClass::Status:
			{
				return L"STATUS";
			}
			case KxUxThemeClass::Tab:
			{
				return L"TAB";
			}
			case KxUxThemeClass::TaskBand:
			{
				return L"TASKBAND";
			}
			case KxUxThemeClass::TaskBar:
			{
				return L"TASKBAR";
			}
			case KxUxThemeClass::TaskDialog:
			{
				return L"TASKDIALOG";
			}
			case KxUxThemeClass::TextStyle:
			{
				return L"TEXTSTYLE";
			}
			case KxUxThemeClass::ToolBar:
			{
				return L"TOOLBAR";
			}
			case KxUxThemeClass::ToolTip:
			{
				return L"TOOLTIP";
			}
			case KxUxThemeClass::TrackBar:
			{
				return L"TRACKBAR";
			}
			case KxUxThemeClass::TrayNotify:
			{
				return L"TRAYNOTIFY";
			}
			case KxUxThemeClass::TreeView:
			{
				return L"TREEVIEW";
			}
			case KxUxThemeClass::Window:
			{
				return L"WINDOW";
			}
		};
		return nullptr;
	}
}

bool KxUxTheme::ClearDC(wxWindow& window, wxDC& dc)
{
	return DrawParentBackground(window, dc, wxRect(wxPoint(0, 0), dc.GetSize()));
}
bool KxUxTheme::DrawParentBackground(wxWindow& window, wxDC& dc)
{
	::DrawThemeParentBackground(window.GetHandle(), dc.GetHDC(), nullptr);
	return true;
}
bool KxUxTheme::DrawParentBackground(wxWindow& window, wxDC& dc, const wxRect& rect)
{
	RECT rectWin = KxUtility::CopyRectToRECT(rect);
	::DrawThemeParentBackground(window.GetHandle(), dc.GetHDC(), &rectWin);
	return true;
}
KxColor KxUxTheme::GetDialogMainInstructionColor(const wxWindow& window, const wxColour& defaultColor)
{
	if (KxUxTheme theme(const_cast<wxWindow&>(window), L"TEXTSTYLE"); theme.IsOK())
	{
		wxColour color = theme.GetColor(TEXT_MAININSTRUCTION, 0, TMT_TEXTCOLOR, defaultColor);

		// Approximation of caption color for default Aero style
		return color.IsOk() ? color : wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT).ChangeLightness(65);
	}
	return defaultColor;
}

void KxUxTheme::Open(wxWindow& window, const wchar_t* classes, KxUxThemeFlags flags)
{
	DWORD dwFlags = 0;
	if (flags & KxUxThemeFlags::ForceRectSizing)
	{
		dwFlags |= OTD_FORCE_RECT_SIZING;
	}
	if (flags & KxUxThemeFlags::NonClient)
	{
		dwFlags |= OTD_NONCLIENT;
	}

	m_Window = &window;
	if (dwFlags != 0)
	{
		m_Handle = ::OpenThemeDataEx(window.GetHandle(), classes, dwFlags);
	}
	else
	{
		m_Handle = ::OpenThemeData(window.GetHandle(), classes);
	}
}
void KxUxTheme::Close()
{
	if (m_Handle)
	{
		::CloseThemeData(m_Handle);
		m_Handle = nullptr;
	}
	m_Window = nullptr;
}

KxUxTheme::KxUxTheme(wxWindow& window, KxUxThemeClass KxUxThemeClass, KxUxThemeFlags flags)
{
	if (const wchar_t* name = MapKxUxThemeClassToName(KxUxThemeClass))
	{
		Open(window, name, flags);
	}
}

wxSize KxUxTheme::GetPartSize(const wxDC& dc, int iPartId, int iStateId, std::optional<int> sizeVariant) const
{
	const THEMESIZE themeSize = static_cast<THEMESIZE>(sizeVariant ? *sizeVariant : TS_DRAW);

	SIZE size = {};       
	if (::GetThemePartSize(m_Handle, dc.GetHDC(), iPartId, iStateId, nullptr, themeSize, &size) == S_OK)
	{
		return wxSize(size.cx, size.cy);
	}
	return wxDefaultSize;
}
wxRegion KxUxTheme::GetBackgroundRegion(const wxDC& dc, int iPartId, int iStateId, const wxRect& rect) const
{
	HRGN region = nullptr;
	RECT rectWin = KxUtility::CopyRectToRECT(rect);
	if (::GetThemeBackgroundRegion(m_Window->GetHandle(), dc.GetHDC(), iPartId, iStateId, &rectWin, &region) == S_OK)
	{
		return region;
	}
	return KxNullWxRegion;
}
std::optional<wxRect> KxUxTheme::GetBackgroundContentRect(const wxDC& dc, int iPartId, int iStateId, const wxRect& rect) const
{
	RECT rectWin = KxUtility::CopyRectToRECT(rect);
	RECT value = {};
	if (::GetThemeBackgroundContentRect(m_Window->GetHandle(), dc.GetHDC(), iPartId, iStateId, &rectWin, &value) == S_OK)
	{
		return KxUtility::CopyRECTToRect(value);
	}
	return std::nullopt;
}

KxColor KxUxTheme::GetColor(int iPartId, int iStateId, int iPropId, const wxColour& defaultValue) const
{
	COLORREF value = 0;
	if (::GetThemeColor(m_Window->GetHandle(), iPartId, iStateId, iPropId, &value) == S_OK)
	{
		return KxColor().SetCOLORREF(value);
	}
	return defaultValue;
}
wxFont KxUxTheme::GetFont(const wxDC& dc, int iPartId, int iStateId, int iPropId) const
{
	LOGFONTW value = {};
	if (::GetThemeFont(m_Window->GetHandle(), dc.GetHDC(), iPartId, iStateId, iPropId, &value) == S_OK)
	{
		return wxNativeFontInfo(value);
	}
	return wxNullFont;
}
std::optional<bool> KxUxTheme::GetBool(int iPartId, int iStateId, int iPropId) const
{
	BOOL value = FALSE;
	if (::GetThemeBool(m_Window->GetHandle(), iPartId, iStateId, iPropId, &value) == S_OK)
	{
		return value;
	}
	return std::nullopt;
}
std::optional<int> KxUxTheme::GetInt(int iPartId, int iStateId, int iPropId) const
{
	int value = -1;
	if (::GetThemeInt(m_Window->GetHandle(), iPartId, iStateId, iPropId, &value) == S_OK)
	{
		return value;
	}
	return std::nullopt;
}
std::optional<int> KxUxTheme::GetEnum(int iPartId, int iStateId, int iPropId) const
{
	int value = -1;
	if (::GetThemeEnumValue(m_Window->GetHandle(), iPartId, iStateId, iPropId, &value) == S_OK)
	{
		return value;
	}
	return std::nullopt;
}
KxIntVector KxUxTheme::GetIntList(int iPartId, int iStateId, int iPropId) const
{
	INTLIST intList = {};
	if (::GetThemeIntList(m_Window->GetHandle(), iPartId, iStateId, iPropId, &intList) == S_OK)
	{
		KxIntVector items;
		items.reserve(intList.iValueCount);
		for (size_t i = 0; i < static_cast<size_t>(intList.iValueCount); i++)
		{
			items.push_back(intList.iValues[i]);
		}

		return items;
	}
	return {};
}
wxRect KxUxTheme::GetRect(int iPartId, int iStateId, int iPropId) const
{
	RECT value = {};
	if (::GetThemeRect(m_Window->GetHandle(), iPartId, iStateId, iPropId, &value) == S_OK)
	{
		return KxUtility::CopyRECTToRect(value);
	}
	return {};
}
wxPoint KxUxTheme::GetPosition(int iPartId, int iStateId, int iPropId) const
{
	POINT value = {};
	if (::GetThemePosition(m_Window->GetHandle(), iPartId, iStateId, iPropId, &value) == S_OK)
	{
		return wxPoint(value.x, value.y);
	}
	return wxDefaultPosition;
}

bool KxUxTheme::DrawEdge(wxDC& dc, int iPartId, int iStateId, uint32_t edge, uint32_t flags, const wxRect& rect, wxRect* boundingRect)
{
	RECT rectWin = KxUtility::CopyRectToRECT(rect);
	RECT clipRectWin = {};
	if (::DrawThemeEdge(m_Handle, dc.GetHDC(), iPartId, iStateId, &rectWin, edge, flags, &clipRectWin) == S_OK)
	{
		KxUtility::SetIfNotNull(boundingRect, KxUtility::CopyRECTToRect(clipRectWin));
		return true;
	}
	return false;
}
bool KxUxTheme::DrawIcon(wxDC& dc, int iPartId, int iStateId, const wxImageList& imageList, int index, const wxRect& rect, wxRect* boundingRect)
{
	RECT rectWin = KxUtility::CopyRectToRECT(rect);
	if (::DrawThemeIcon(m_Handle, dc.GetHDC(), iPartId, iStateId, &rectWin, reinterpret_cast<HIMAGELIST>(imageList.GetHIMAGELIST()), index) == S_OK)
	{
		if (boundingRect)
		{
			boundingRect->x = rect.x;
			boundingRect->y = rect.y;
			imageList.GetSize(index, boundingRect->width, boundingRect->height);
		}
		return true;
	}
	return false;
}
bool KxUxTheme::DrawText(wxDC& dc, int iPartId, int iStateId, std::wstring_view text, uint32_t flags1, uint32_t flags2, const wxRect& rect)
{
	RECT rectWin = KxUtility::CopyRectToRECT(rect);
	return ::DrawThemeText(m_Handle, dc.GetHDC(), iPartId, iStateId, text.data(), text.length(), flags1, flags2, &rectWin) == S_OK;
}

bool KxUxTheme::DrawBackground(wxDC& dc, int iPartId, int iStateId, const wxRect& rect)
{
	RECT rectWin = KxUtility::CopyRectToRECT(rect);
	return ::DrawThemeBackground(m_Handle, dc.GetHDC(), iPartId, iStateId, &rectWin, nullptr) == S_OK;
}
bool KxUxTheme::DrawProgress(wxDC& dc, int iBarPartId, int iFillPartId, int iFillStateId, const wxRect& rect, int position, int range, KxColor* averageBackgroundColor)
{
	bool result = true;

	// Draw background part
	wxRect fillRect = rect;
	if (iBarPartId > 0)
	{
		result = DrawBackground(dc, iBarPartId, 0, rect);
		fillRect = GetBackgroundContentRect(dc, iBarPartId, 0, rect).value_or(rect);
	}

	// Draw filled part
	if (iFillStateId > 0)
	{
		if (position != range)
		{
			fillRect.SetWidth(fillRect.GetWidth() * ((double)position / (double)range));
		}
		result = DrawBackground(dc, iFillPartId, iFillStateId, fillRect);

		if (averageBackgroundColor)
		{
			*averageBackgroundColor = KxDCUtility::GetAverageColor(dc, fillRect);
		}
	}
	return result;
}

KxUxTheme& KxUxTheme::operator=(KxUxTheme&& other)
{
	Close();

	m_Handle = KxUtility::ExchangeResetAndReturn(other.m_Handle, nullptr);
	m_Window = KxUtility::ExchangeResetAndReturn(other.m_Window, nullptr);
	return *this;
}
