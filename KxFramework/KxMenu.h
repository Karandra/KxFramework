#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxMenuEvent.h"
#include "KxFramework/KxMenuItem.h"

class KX_API KxMenu: public wxMenu
{
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
		virtual WORD DoShowMenu(wxWindow* window, const wxPoint& pos, DWORD alignment, bool async);
		void ProcessItemSelection(int menuWxID, wxCommandEvent* event = nullptr);

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
		static const DWORD DefaultAlignment = TPM_LEFTALIGN|TPM_TOPALIGN;

		KxMenu(const wxString& label = {}, long style = 0);
		virtual ~KxMenu();

	public:
		virtual bool IsOK() const
		{
			return true;
		}

	public:
		wxWindowID Show(wxWindow* window = nullptr, const wxPoint& pos = wxDefaultPosition, DWORD alignment = DefaultAlignment);
		wxWindowID ShowAsPopup(wxWindow* window, int offset = 1, DWORD alignment = DefaultAlignment);

	public:
		virtual KxMenuItem* Add(KxMenuItem* item);
		KxMenuItem* Add(KxMenu* subMenu, const wxString& label, const wxString& helpString = {});
		KxMenuItem* AddSeparator();

		virtual KxMenuItem* Insert(size_t pos, KxMenuItem* item);
		KxMenuItem* Insert(size_t pos, KxMenu* subMenu, const wxString& label, const wxString& helpString = {});
		KxMenuItem* InsertSeparator(size_t pos);

		virtual KxMenuItem* Prepend(KxMenuItem* item);
		KxMenuItem* Prepend(KxMenu* subMenu, const wxString& label, const wxString& helpString = {});
		KxMenuItem* PrependSeparator();

		KxMenuItem* FindChildItem(wxWindowID id, size_t* posPtr = nullptr) const;
		KxMenuItem* FindItem(wxWindowID id, KxMenu** menu = nullptr) const;
		KxMenuItem* FindItemByPosition(size_t pos) const;

		virtual KxMenuItem* RemoveItem(KxMenuItem* item);
		virtual KxMenuItem* RemoveItem(wxWindowID id);

	public:
		template<class TItem = KxMenuItem, class... Args> TItem* AddItem(Args&&... arg)
		{
			TItem* item = new TItem(std::forward<Args>(arg)...);
			Add(item);
			return item;
		}
		template<class TItem = KxMenuItem, class... Args> TItem* InsertItem(size_t pos, Args&& ... arg)
		{
			TItem* item = new TItem(std::forward<Args>(arg)...);
			Insert(pos, item);
			return item;
		}
		template<class TItem = KxMenuItem, class... Args> TItem* PrependItem(Args&& ... arg)
		{
			TItem* item = new TItem(std::forward<Args>(arg)...);
			Prepend(item);
			return item;
		}

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
