#pragma once
#include "KxStdAfx.h"
#include <KxEnumClassOperations.h>

// https://docs.microsoft.com/ru-ru/windows/win32/controls/uxctl-ref
// https://docs.microsoft.com/ru-ru/windows/win32/controls/theme-subclasses
enum class KxUxThemeFlags: uint32_t
{
	None = 0,
	NonClient = 1 << 0,
	ForceRectSizing = 1 << 1,
};
enum class KxUxThemeClass
{
	Button,
	Clock,
	ComboBox,
	Communications,
	ControlPanel,
	DatePicker,
	DragDrop,
	Edit,
	ExplorerBar,
	FlyOut,
	Globals,
	Header,
	ListBox,
	ListView,
	Menu,
	MenuBand,
	Navigation,
	Page,
	Progress,
	Rebar,
	ScrollBar,
	SearchEditBox,
	Spin,
	StartPanel,
	Status,
	Tab,
	TaskBand,
	TaskBar,
	TaskDialog,
	TextStyle,
	ToolBar,
	ToolTip,
	TrackBar,
	TrayNotify,
	TreeView,
	Window
};

class KxUxTheme final
{
	public:
		static bool ClearDC(wxWindow& window, wxDC& dc);
		static bool DrawParentBackground(wxWindow& window, wxDC& dc);
		static bool DrawParentBackground(wxWindow& window, wxDC& dc, const wxRect& rect);
		static KxColor GetDialogMainInstructionColor(const wxWindow& window, const wxColour& defaultColor = wxNullColour);

	private:
		void* m_Handle = nullptr;
		wxWindow* m_Window = nullptr;

	private:
		void Open(wxWindow& window, const wchar_t* classes, KxUxThemeFlags flags = KxUxThemeFlags::None);
		void Close();

	public:
		KxUxTheme() = default;
		KxUxTheme(wxWindow& window, const wchar_t* classes, KxUxThemeFlags flags = KxUxThemeFlags::None)
		{
			Open(window, classes, flags);
		}
		KxUxTheme(wxWindow& window, const wxString& classes, KxUxThemeFlags flags = KxUxThemeFlags::None)
		{
			Open(window, classes.wc_str(), flags);
		}
		KxUxTheme(wxWindow& window, KxUxThemeClass themeClass, KxUxThemeFlags flags = KxUxThemeFlags::None);
		KxUxTheme(const KxUxTheme&) = delete;
		KxUxTheme(KxUxTheme&& other)
		{
			*this = std::move(other);
		}
		~KxUxTheme()
		{
			Close();
		}

	public:
		bool IsOK() const
		{
			return m_Handle && m_Window;
		}
		void* GetHandle() const
		{
			return m_Handle;
		}
		wxWindow& GetWindow() const
		{
			return *m_Window;
		}

		wxSize GetPartSize(const wxDC& dc, int iPartId, int iStateId, std::optional<int> sizeVariant = {}) const;
		wxRegion GetBackgroundRegion(const wxDC& dc, int iPartId, int iStateId, const wxRect& rect) const;
		std::optional<wxRect> GetBackgroundContentRect(const wxDC& dc, int iPartId, int iStateId, const wxRect& rect) const;

		KxColor GetColor(int iPartId, int iStateId, int iPropId, const wxColour& defaultValue = wxNullColour) const;
		wxFont GetFont(const wxDC& dc, int iPartId, int iStateId, int iPropId) const;
		std::optional<bool> GetBool(int iPartId, int iStateId, int iPropId) const;
		std::optional<int> GetInt(int iPartId, int iStateId, int iPropId) const;
		std::optional<int> GetEnum(int iPartId, int iStateId, int iPropId) const;
		KxIntVector GetIntList(int iPartId, int iStateId, int iPropId) const;
		wxRect GetRect(int iPartId, int iStateId, int iPropId) const;
		wxPoint GetPosition(int iPartId, int iStateId, int iPropId) const;

		bool DrawEdge(wxDC& dc, int iPartId, int iStateId, uint32_t edge, uint32_t flags, const wxRect& rect, wxRect* boundingRect = nullptr);
		bool DrawIcon(wxDC& dc, int iPartId, int iStateId, const wxImageList& imageList, int index, const wxRect& rect, wxRect* boundingRect = nullptr);
		bool DrawText(wxDC& dc, int iPartId, int iStateId, std::wstring_view text, uint32_t flags1, uint32_t flags2, const wxRect& rect);

		bool DrawBackground(wxDC& dc, int iPartId, int iStateId, const wxRect& rect);
		bool DrawParentBackground(wxDC& dc)
		{
			return DrawParentBackground(*m_Window, dc);
		}
		bool DrawParentBackground(wxDC& dc, const wxRect& rect)
		{
			return DrawParentBackground(*m_Window, dc, rect);
		}
		bool DrawProgressBar(wxDC& dc, int iBarPartId, int iFillPartId, int iFillStateId, const wxRect& rect, int position, int range, KxColor* averageBackgroundColor = nullptr);

	public:
		explicit operator bool() const
		{
			return IsOK();
		}
		bool operator!() const
		{
			return !IsOK();
		}
		
		KxUxTheme& operator=(const KxUxTheme&) = delete;
		KxUxTheme& operator=(KxUxTheme&& other);
};

namespace KxEnumClassOperations
{
	KxImplementEnum(KxUxThemeFlags);
	KxImplementEnum(KxUxThemeClass);
}
