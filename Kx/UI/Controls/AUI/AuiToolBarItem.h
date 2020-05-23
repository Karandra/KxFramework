#pragma once
#include "Kx/UI/Common.h"
#include "Kx/General/WithOptions.h"
#include "Kx/UI/WithDropdownMenu.h"
#include <wx/aui/auibar.h>

namespace KxFramework::UI
{
	class AuiToolBar;
}

namespace KxFramework::UI
{
	enum class AuiToolBarItemOption
	{
		None = 0,
		MenuOnLeftClick = 1 << 0,
		MenuOnMiddleClick = 1 << 1,
		MenuOnRightClick = 1 << 2,
	};
}

namespace KxFramework::UI
{
	class KX_API AuiToolBarItem:
		public wxEvtHandler,
		public WithDropdownMenu,
		public WithOptions<AuiToolBarItemOption, AuiToolBarItemOption::None>
	{
		friend class AuiToolBar;

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			AuiToolBar* m_ToolBar = nullptr;
			wxAuiToolBarItem* m_Item = nullptr;

		private:
			Point DoGetDropdownMenuPosition(wxAlignment* alignment, bool leftAlign) const;
			wxWindowID DoShowDropdownMenu(bool leftAlign);

		public:
			AuiToolBarItem() = default;
			AuiToolBarItem(AuiToolBar& control, wxAuiToolBarItem& item)
				:m_ToolBar(&control), m_Item(&item)
			{
			}

		public:
			bool IsOK() const noexcept
			{
				return m_ToolBar != nullptr && m_Item != nullptr;
			}
			void Refresh();
			AuiToolBar& GetToolBar() const
			{
				return *m_ToolBar;
			}

			Point GetDropdownMenuPosition(wxAlignment* alignment = nullptr) const
			{
				return DoGetDropdownMenuPosition(alignment, !HasDropDown());
			}
			Point GetDropdownMenuPosLeftAlign(wxAlignment* alignment = nullptr) const
			{
				return DoGetDropdownMenuPosition(alignment, true);
			}
			Point GetDropdownMenuPosRightAlign(wxAlignment* alignment = nullptr) const
			{
				return DoGetDropdownMenuPosition(alignment, false);
			}
		
			wxWindowID ShowDropdownMenu()
			{
				return DoShowDropdownMenu(!HasDropDown());
			}
			wxWindowID ShowDropdownMenuLeftAlign()
			{
				return DoShowDropdownMenu(true);
			}
			wxWindowID ShowDropdownMenuRightAlign()
			{
				return DoShowDropdownMenu(false);
			}

			wxWindowID GetID() const;
			Rect GetRect() const;
			bool IsItemFits() const;

			int GetIndex() const;
			bool SetIndex(size_t index);

			bool IsToggled() const;
			void SetToggled(bool isPressed);

			bool IsEnabled() const;
			void SetEnabled(bool isEnabled);

			bool HasDropDown() const;
			void SetDropDown(bool isDropDown);

			bool IsSticky() const;
			void SetSticky(bool isSticky);

			void SetActive(bool isActive);
			bool IsActive() const;

			int GetProportion() const;
			void SetProportion(int proportion);

			wxAlignment GetAlignment() const;
			void SetAlignment(wxAlignment alignment);

			int GetSpacerPixels() const;
			void SetSpacerPixels(int pixels);

			void SetKind(wxItemKind kind);
			wxItemKind GetKind() const;

			void SetWindow(wxWindow* window);
			wxWindow* GetWindow();

			String GetLabel() const;
			void SetLabel(const String& label);

			String GetShortHelp() const;
			void SetShortHelp(const String& helpString);

			String GetLongHelp() const;
			void SetLongHelp(const String& helpString);

			const wxBitmap& GetBitmap() const;
			void SetBitmap(const wxBitmap& bitmap);

			const wxBitmap& GetDisabledBitmap() const;
			void SetDisabledBitmap(const wxBitmap& bitmap);

		public:
			explicit operator bool() const noexcept
			{
				return IsOK();
			}
			bool operator!() const noexcept
			{
				return !IsOK();
			}
	};
}
