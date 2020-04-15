#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/General/WithOptions.h"
#include "Kx/UI/WithDropdownMenu.h"
#include <wx/aui/auibar.h>
class KxAuiToolBar;

enum KxAuiToolBarItem_Options
{
	KxAUI_TBITEM_OPTION_NONE = 0,
	KxAUI_TBITEM_OPTION_LCLICK_MENU = 1 << 0,
	KxAUI_TBITEM_OPTION_MCLICK_MENU = 1 << 1,
	KxAUI_TBITEM_OPTION_RCLICK_MENU = 1 << 2,
};

class KX_API KxAuiToolBarItem:
	public wxEvtHandler,
	public KxFramework::WithDropdownMenu,
	public KxFramework::WithOptions<KxAuiToolBarItem_Options, KxAUI_TBITEM_OPTION_NONE>
{
	friend class KxAuiToolBar;

	public:
		static constexpr size_t npos = std::numeric_limits<size_t>::max();

	private:
		KxAuiToolBar* m_ToolBar = nullptr;
		wxAuiToolBarItem* m_Item = nullptr;

	private:
		wxPoint DoGetDropdownMenuPosition(DWORD* alignment, bool leftAlign) const;
		wxWindowID DoShowDropdownMenu(bool leftAlign);

	public:
		KxAuiToolBarItem() = default;
		KxAuiToolBarItem(KxAuiToolBar& control, wxAuiToolBarItem& item);
		~KxAuiToolBarItem();

	public:
		bool IsOK() const;
		void Refresh();
		KxAuiToolBar& GetToolBar() const
		{
			return *m_ToolBar;
		}

		wxPoint GetDropdownMenuPosition(DWORD* alignment = nullptr) const
		{
			return DoGetDropdownMenuPosition(alignment, !HasDropDown());
		}
		wxPoint GetDropdownMenuPosLeftAlign(DWORD* alignment = nullptr) const
		{
			return DoGetDropdownMenuPosition(alignment, true);
		}
		wxPoint GetDropdownMenuPosRightAlign(DWORD* alignment = nullptr) const
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
		wxRect GetRect() const;
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

		const wxString& GetLabel() const;
		void SetLabel(const wxString& label);

		const wxString& GetShortHelp() const;
		void SetShortHelp(const wxString& helpString);

		const wxString& GetLongHelp() const;
		void SetLongHelp(const wxString& helpString);

		const wxBitmap& GetBitmap() const;
		void SetBitmap(const wxBitmap& bitmap);

		const wxBitmap& GetDisabledBitmap() const;
		void SetDisabledBitmap(const wxBitmap& bitmap);
};
