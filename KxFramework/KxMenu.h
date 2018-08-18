#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxMenuEvent.h"
#include "KxFramework/KxMenuItem.h"

class KxMenu: public wxMenu
{
	private:
		static KxMenu* m_CurrentMenu;

	public:
		static bool EndMenu();
		static KxMenu* GetCurrentMenu();

	public:
		using iterator = KxMenuItemIterator<KxMenuItem, KxMenu>;
		using const_iterator = KxMenuItemIterator<const KxMenuItem, const KxMenu>;

	private:
		#if 0
		wxMenuItem* Append(wxMenuItem*) = delete;
		wxMenuItem* Append(int, const wxString&, wxMenu*, const wxString&) = delete;
		wxMenuItem* Append(int, const wxString&, wxMenu*, const wxString&) = delete;
		wxMenuItem* AppendCheckItem(int, const wxString&, const wxString&) = delete;
		wxMenuItem* AppendRadioItem(int, const wxString&, const wxString&) = delete;
		wxMenuItem* AppendSeparator() = delete;
		wxMenuItem* AppendSubMenu(wxMenu*, const wxString&, const wxString&) = delete;

		wxMenuItem* Insert(size_t, wxMenuItem*) = delete;
		wxMenuItem* Insert(size_t, int, const wxString&, const wxString&, wxItemKind) = delete;
		wxMenuItem* Insert(size_t, int, const wxString&, wxMenu*, const wxString&) = delete;
		wxMenuItem* InsertCheckItem(size_t, int, const wxString&, const wxString&) = delete;
		wxMenuItem* InsertRadioItem(size_t, int, const wxString&, const wxString&) = delete;
		//wxMenuItem* InsertSeparator(size_t) = delete;

		wxMenuItem* Prepend(wxMenuItem*) = delete;
		wxMenuItem* Prepend(int, const wxString&, wxMenu*, const wxString&) = delete;
		wxMenuItem* Prepend(int, const wxString&, wxMenu*, const wxString&) = delete;
		wxMenuItem* PrependCheckItem(int, const wxString&, const wxString&) = delete;
		wxMenuItem* PrependRadioItem(int, const wxString&, const wxString&) = delete;
		//wxMenuItem* PrependSeparator(size_t) = delete;

		wxMenuItem* Remove(wxMenuItem*) = delete;
		wxMenuItem* Remove(int) = delete;

		wxMenuItemList& GetMenuItems() = delete;
		const wxMenuItemList& GetMenuItems() const = delete;
		#endif

	private:
		void OnSelectItem(wxCommandEvent& event);
		void OnHoverItem(wxMenuEvent& event);

	protected:
		KxMenuItem* FindItemByEvent(const wxEvent& event, KxMenu** menu = NULL);
		void wxYieldForCommandsOnly();
		WORD ShowNoEvent(wxWindow* window = NULL, const wxPoint& pos = wxDefaultPosition, DWORD alignment = DefaultAlignment);
		void ProcessItemSelection(int menuWxID, wxCommandEvent* event = NULL);
		void ProcessItemSelection(WORD menuWinID, wxCommandEvent* event = NULL)
		{
			ProcessItemSelection(WinIDToWx(menuWinID));
		}

	public:
		static int WinIDToWx(UINT menuWinID)
		{
			return (signed short)menuWinID;
		}
		static UINT WxIDToWin(int menuWxID)
		{
			return (UINT)menuWxID;
		}
		static wxWindowID WinMenuRetToWx(WORD menuWinID)
		{
			if (menuWinID != 0)
			{
				return WinIDToWx(menuWinID);
			}
			return wxID_NONE;
		}

	public:
		static const DWORD AlignmentMask = TPM_LEFTALIGN|TPM_CENTERALIGN|TPM_RIGHTALIGN|TPM_TOPALIGN|TPM_VCENTERALIGN|TPM_BOTTOMALIGN|TPM_HORIZONTAL|TPM_VERTICAL;
		static const DWORD DefaultAlignment = TPM_LEFTALIGN|TPM_TOPALIGN;

		KxMenu(const wxString& label = wxEmptyString, long style = 0);
		virtual ~KxMenu();

	public:
		virtual wxWindowID Show(wxWindow* window = NULL, const wxPoint& pos = wxDefaultPosition, DWORD alignment = DefaultAlignment);
		virtual wxWindowID ShowAsPopup(wxWindow* window, int offset = 1, DWORD alignment = DefaultAlignment);

		virtual KxMenuItem* Add(KxMenuItem* item);
		KxMenuItem* Add(KxMenu* subMenu, const wxString& label, const wxString& helpString = wxEmptyString);
		KxMenuItem* AddSeparator();

		virtual KxMenuItem* Insert(size_t pos, KxMenuItem* item);
		KxMenuItem* Insert(size_t pos, KxMenu* subMenu, const wxString& label, const wxString& helpString = wxEmptyString);
		KxMenuItem* InsertSeparator(size_t pos);

		virtual KxMenuItem* Prepend(KxMenuItem* item);
		KxMenuItem* Prepend(KxMenu* subMenu, const wxString& label, const wxString& helpString = wxEmptyString);
		KxMenuItem* PrependSeparator();

		KxMenuItem* FindChildItem(wxWindowID id, size_t* nPosPtr = NULL) const;
		KxMenuItem* FindItem(wxWindowID id, KxMenu** menu = NULL) const;
		KxMenuItem* FindItemByPosition(size_t pos) const;

		virtual KxMenuItem* RemoveItem(KxMenuItem* item);
		virtual KxMenuItem* RemoveItem(wxWindowID id);

	public:
		iterator begin()
		{
			return iterator(this);
		}
		iterator end()
		{
			return iterator(this, GetMenuItemCount());
		}

		const_iterator begin() const
		{
			return const_iterator(this);
		}
		const_iterator end() const
		{
			return const_iterator(this, GetMenuItemCount());
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxMenu);
};
