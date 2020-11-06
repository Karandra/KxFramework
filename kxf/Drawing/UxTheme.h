#pragma once
#include "Common.h"
#include "Color.h"
#include "Geometry.h"
#include "kxf/General/String.h"
#include <wx/imaglist.h>
#include <wx/window.h>
#include <wx/gdicmn.h>
#include <wx/dc.h>
#include <wx/dcgraph.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/renderer.h>

// https://docs.microsoft.com/ru-ru/windows/win32/controls/uxctl-ref
// https://docs.microsoft.com/ru-ru/windows/win32/controls/theme-subclasses
namespace kxf
{
	enum class UxThemeFlag: uint32_t
	{
		None = 0,

		NonClient = 1 << 0,
		ForceRectSizing = 1 << 1,
	};
	KxFlagSet_Declare(UxThemeFlag);

	enum class UxThemeClass
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
}

namespace kxf
{
	class UxTheme final
	{
		public:
			static bool ClearDC(wxWindow& window, wxDC& dc) noexcept;
			static bool DrawParentBackground(wxWindow& window, wxDC& dc) noexcept;
			static bool DrawParentBackground(wxWindow& window, wxDC& dc, const Rect& rect) noexcept;

			static Color GetDialogMainInstructionColor(const wxWindow& window) noexcept;

		private:
			void* m_Handle = nullptr;
			wxWindow* m_Window = nullptr;

		private:
			void Open(wxWindow& window, const wchar_t* classes, FlagSet<UxThemeFlag> flags = {}) noexcept;
			void Close() noexcept;

		public:
			UxTheme() noexcept = default;
			UxTheme(wxWindow& window, const wchar_t* classes, FlagSet<UxThemeFlag> flags = {}) noexcept
			{
				Open(window, classes, flags);
			}
			UxTheme(wxWindow& window, const String& classes, FlagSet<UxThemeFlag> flags = {}) noexcept
			{
				Open(window, classes.wc_str(), flags);
			}
			UxTheme(wxWindow& window, UxThemeClass themeClass, FlagSet<UxThemeFlag> flags = {}) noexcept;
			UxTheme(const UxTheme&) = delete;
			UxTheme(UxTheme&& other) noexcept
			{
				*this = std::move(other);
			}
			~UxTheme() noexcept
			{
				Close();
			}

		public:
			bool IsNull() const noexcept
			{
				return !m_Handle || !m_Window;
			}
			void* GetHandle() const noexcept
			{
				return m_Handle;
			}
			wxWindow& GetWindow() const noexcept
			{
				return *m_Window;
			}

			Size GetPartSize(const wxDC& dc, int iPartId, int iStateId, std::optional<int> sizeVariant = {}) const noexcept;
			wxRegion GetBackgroundRegion(const wxDC& dc, int iPartId, int iStateId, const Rect& rect) const noexcept;
			std::optional<Rect> GetBackgroundContentRect(const wxDC& dc, int iPartId, int iStateId, const Rect& rect) const noexcept;

			Color GetColor(int iPartId, int iStateId, int iPropId) const noexcept;
			wxFont GetFont(const wxDC& dc, int iPartId, int iStateId, int iPropId) const noexcept;
			std::optional<bool> GetBool(int iPartId, int iStateId, int iPropId) const noexcept;
			std::optional<int> GetInt(int iPartId, int iStateId, int iPropId) const noexcept;
			std::optional<int> GetEnum(int iPartId, int iStateId, int iPropId) const noexcept;
			size_t GetIntList(int iPartId, int iStateId, int iPropId, std::function<bool(int)> func) const;
			Rect GetRect(int iPartId, int iStateId, int iPropId) const noexcept;
			Point GetPosition(int iPartId, int iStateId, int iPropId) const noexcept;

			bool DrawEdge(wxDC& dc, int iPartId, int iStateId, uint32_t edge, uint32_t flags, const Rect& rect, Rect* boundingRect = nullptr) noexcept;
			bool DrawIcon(wxDC& dc, int iPartId, int iStateId, const wxImageList& imageList, int index, const Rect& rect, Rect* boundingRect = nullptr) noexcept;
			bool DrawText(wxDC& dc, int iPartId, int iStateId, std::wstring_view text, uint32_t flags1, uint32_t flags2, const Rect& rect) noexcept;

			bool DrawBackground(wxDC& dc, int iPartId, int iStateId, const Rect& rect) noexcept;
			bool DrawParentBackground(wxDC& dc) noexcept
			{
				return DrawParentBackground(*m_Window, dc);
			}
			bool DrawParentBackground(wxDC& dc, const Rect& rect) noexcept
			{
				return DrawParentBackground(*m_Window, dc, rect);
			}
			bool DrawProgressBar(wxDC& dc, int iBarPartId, int iFillPartId, int iFillStateId, const Rect& rect, int position, int range, Color* averageBackgroundColor = nullptr) noexcept;

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			UxTheme& operator=(const UxTheme&) = delete;
			UxTheme& operator=(UxTheme&& other) noexcept;
	};
}
