#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithDropdownMenu.h"
#include "KxFramework/KxWithOptions.h"
#include <wx/aui/auibar.h>
class KxAuiToolBar;

enum KxAuiToolBarItem_Options
{
	KxAUI_TBITEM_OPTION_NONE = 0,
	KxAUI_TBITEM_OPTION_LCLICK_MENU = 1 << 0,
	KxAUI_TBITEM_OPTION_MCLICK_MENU = 1 << 1,
	KxAUI_TBITEM_OPTION_RCLICK_MENU = 1 << 2,
};

class KxAuiToolBarItem:
	public wxEvtHandler,
	public KxWithDropdownMenu,
	public KxWithOptions<KxAuiToolBarItem_Options, KxAUI_TBITEM_OPTION_NONE>
{
	friend class KxAuiToolBar;

	private:
		KxAuiToolBar* m_Control = NULL;
		wxAuiToolBarItem* m_Item = NULL;

	public:
		KxAuiToolBarItem(KxAuiToolBar* control, wxAuiToolBarItem* item);
		virtual ~KxAuiToolBarItem();

	public:
		bool IsOK() const;
		void Refresh();

		KxAuiToolBar* GetToolBar() const;
		wxPoint GetDropdownMenuPosition(DWORD* alignment = NULL) const;
		wxWindowID ShowDropdownMenu();
		wxWindowID GetID() const;
		wxRect GetRect() const;
		int GetPosition() const;
		int GetIndex() const;
		bool IsItemFits() const;

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
using KxAuiToolBarItem_ArrayElement = std::pair<wxWindowID, KxAuiToolBarItem*>;
using KxAuiToolBarItem_Array = std::vector<KxAuiToolBarItem_ArrayElement>;
using KxAuiToolBarItem_Iterator = KxAuiToolBarItem_Array::iterator;
using KxAuiToolBarItem_ConstIterator = KxAuiToolBarItem_Array::const_iterator;
