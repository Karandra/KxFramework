#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"
#include "KxEvent.h"

KxEVENT_DECLARE_GLOBAL(wxTreeEvent, TREE_ITEM_CHECKED);

enum KxTreeView_ItemState
{
	KxTV_STATE_CHECKED = 1,
	KxTV_STATE_UNCHECKED = 0,
	KxTV_STATE_INDETERMINATE = 2,
	KxTV_STATE_HIDE = wxTREE_ITEMSTATE_NONE,
};
enum KxTreeView_Options
{
	KxTV_NONE = 0,
	KxTV_LASTSTD = wxTR_FULL_ROW_HIGHLIGHT,

	KxTV_CHK_ENABLED = KxTV_LASTSTD << 1,
	KxTV_CHK_3STATE = KxTV_LASTSTD << 2,
	KxTV_CHK_CHANGE_STATE = KxTV_LASTSTD << 3,
	KxTV_HOTTARCK_ENABLED = KxTV_LASTSTD << 4,
	KxTV_BUTTONS_FADE_ENABLED = KxTV_LASTSTD << 5,

	KxTV_MASK = KxTV_CHK_ENABLED|KxTV_CHK_3STATE|KxTV_CHK_CHANGE_STATE|KxTV_HOTTARCK_ENABLED|KxTV_BUTTONS_FADE_ENABLED
};

class KX_API KxTextBox;
class KX_API KxTreeView;
class KX_API KxTreeViewItem
{
	private:
		KxTreeView* m_Control = nullptr;
		wxTreeItemId m_Item;

	private:
		void OnCreate();

	public:
		static const int NO_IMAGE = wxWithImages::NO_IMAGE;

		KxTreeViewItem() {}
		KxTreeViewItem(KxTreeView& control, const wxTreeItemId& v);
		void Create(KxTreeView& control, const wxTreeItemId& v);
		~KxTreeViewItem() {}

	public:
		bool IsOK() const
		{
			return m_Item.IsOk();
		}
		KxTreeView& GetControl()
		{
			return *m_Control;
		}

		KxTreeViewItem Add(const wxString& label, wxTreeItemData* data = nullptr);
		KxTreeViewItem Insert(const wxString& label, KxTreeViewItem& previous, wxTreeItemData* data = nullptr);
		KxTreeViewItem Insert(const wxString& label, size_t index, wxTreeItemData* data = nullptr);

		KxTreeViewItem GetFirstChild(wxTreeItemIdValue& cookie) const;
		KxTreeViewItem GetNextChild(wxTreeItemIdValue& cookie) const;
		KxTreeViewItem GetNextVisible() const;
		KxTreeViewItem GetPrevSibling() const;
		KxTreeViewItem GetNextSibling() const;
		KxTreeViewItem GetParent() const;

		void Remove();
		void RemoveChildren();
		void ExpandChildren();
		void SortChildren();
		void CollapseChildren();
		void Expand()
		{
			SetExpanded(true);
		}
		void Collape()
		{
			SetExpanded(false);
		}
		void SetExpanded(bool expanded);
		void EnsureVisible();
		void SetFocus();
		void SetSelection(bool b = true);
		void SetHasChildren(bool b = true);
		bool HasChildren() const
		{
			wxTreeItemIdValue cookie;
			return GetFirstChild(cookie).IsOK();
		}

		wxFont GetFont() const;
		void SetFont(const wxFont& f);
		wxColour GetBackColor() const;
		void SetBackColor(const wxColour& c);
		wxColour GetForeColor() const;
		void SetForeColor(const wxColour& c);

		wxString GetLabel() const;
		void SetLabel(const wxString& s);

		KxTextBox* BeginEditLabel();
		void EndEditLabel(bool discardChnages = false);

		int GetState() const;
		void SetState(int state);

		int GetImage(wxTreeItemIcon type = wxTreeItemIcon_Normal) const;
		void SetImage(wxTreeItemIcon type = wxTreeItemIcon_Normal, int nImage = NO_IMAGE);

		bool IsBold() const;
		void SetBold(bool b);

		wxTreeItemData* GetData() const;
		void SetData(wxTreeItemData* p);

		bool IsChecked() const;
		void SetChecked(bool b);

		wxCheckBoxState GetChecked() const;
		void SetChecked(wxCheckBoxState v);

	public:
		operator const wxTreeItemId() const
		{
			return m_Item;
		}
		operator wxTreeItemId&()
		{
			return m_Item;
		}
};

class KX_API KxTreeView: public wxTreeCtrl
{
	friend class KxTreeViewItem;

	public:
		typedef void* HTREEITEM;

	private:
		static wxImageList ms_CheckBoxIcons;
		static bool ms_CheckBoxIconsInitialized;
		static void InitCheckBoxImages(wxWindow* window);
		static HTREEITEM GetHITEM(const wxTreeItemId& itemID)
		{
			return (HTREEITEM)itemID.m_pItem;
		}

	private:
		KxTreeView_Options m_Options = KxTV_NONE;

	private:
		void OnKillFocus(wxFocusEvent& event);
		void OnCheck(wxTreeEvent& event);
		void OnKeyDown(wxTreeEvent& event);

		wxTreeItemId AddRoot() = delete;
		wxTreeItemId GetRootItem() = delete;
		void ExpandAll() = delete;
		void CollapseAll() = delete;

		void SetFocusedItem(const wxTreeItemId& item)
		{
			wxTreeCtrl::SetFocusedItem(item);
		}

	public:
		static const long DefaultStyle = wxTR_NO_LINES|wxTR_LINES_AT_ROOT|wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|KxTV_HOTTARCK_ENABLED;
		static const int DefaultItemHeight = 21;

		KxTreeView() {}
		KxTreeView(wxWindow* parent,
				   wxWindowID id,
				   long style = DefaultStyle
		)
		{
			Create(parent, id, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle
		);
		~KxTreeView();

	public:
		int GetItemHeight() const;
		void SetItemHeight(int height);

		bool IsCheckBoxesEnabled()
		{
			return m_Options & KxTV_CHK_ENABLED;
		}
		void SetCheckBoxesEnabled(bool enable)
		{
			wxTreeCtrl::SetStateImageList(enable ? &ms_CheckBoxIcons : nullptr);
			KxUtility::ModFlagRef(m_Options, KxTV_CHK_ENABLED, enable);
			SetStateImagesChangeEnabled(enable);
		}
		
		bool Is3StateCheckboxesEnabled()
		{
			return m_Options & KxTV_CHK_3STATE;
		}
		void Set3StateCheckboxesEnabled(bool enable)
		{
			KxUtility::ModFlagRef(m_Options, KxTV_CHK_3STATE, enable);
		}
		
		bool IsHotTrackEnabled()
		{
			return m_Options & KxTV_HOTTARCK_ENABLED;;
		}
		void SetHotTrackEnabled(bool enable);
		
		bool IsFadeButtonsEnabled()
		{
			return m_Options & KxTV_BUTTONS_FADE_ENABLED;
		}
		void SetFadeButtonsEnabled(bool enable);
		
		bool IsStateImagesChangeEnabled()
		{
			return m_Options & KxTV_CHK_CHANGE_STATE;
		}
		void SetStateImagesChangeEnabled(bool enable)
		{
			KxUtility::ModFlagRef(m_Options, KxTV_CHK_CHANGE_STATE, enable);
		}

		int IsCheckable(const wxTreeItemId& itemID) const
		{
			return GetItemState(itemID) != KxTV_STATE_HIDE;
		}
		KxTreeView_ItemState GetItemChecked(const wxTreeItemId& itemID) const
		{
			return (KxTreeView_ItemState)GetItemState(itemID);
		}
		bool IsItemChecked(const wxTreeItemId& itemID) const
		{
			return GetItemState(itemID) == KxTV_STATE_CHECKED;
		}
		void SetItemChecked(const wxTreeItemId& itemID, KxTreeView_ItemState state)
		{
			if (state == KxTV_STATE_CHECKED || state == KxTV_STATE_UNCHECKED || state == KxTV_STATE_INDETERMINATE || state == KxTV_STATE_HIDE)
			{
				SetItemState(itemID, state);
			}
		}
		void SetItemChecked(const wxTreeItemId& itemID, bool isChecked)
		{
			SetItemState(itemID, isChecked ? KxTV_STATE_CHECKED : KxTV_STATE_UNCHECKED);
		}

		virtual void SetStateImageList(wxImageList* pImageList) override
		{
			SetCheckBoxesEnabled(false);
			wxTreeCtrl::SetStateImageList(pImageList);
		}

		KxTreeViewItem InsertRoot(const wxString& label, wxTreeItemData* data = nullptr);
		KxTreeViewItem GetRoot() const;
		KxTreeViewItem GetFirstVisible() const;
		KxTreeViewItem GetFocused() const;
		KxTreeViewItem GetSelectedItem() const;
		KxTreeViewItem HitTestItem(const wxPoint& pos, int& flags) const;

		void ClearItems()
		{
			wxTreeCtrl::DeleteAllItems();
		}
		size_t GetItemsCount() const
		{
			return GetCount();
		}
		void ExpandAllItems()
		{
			wxTreeCtrl::ExpandAll();
		}
		void CollapseAllItems()
		{
			wxTreeCtrl::CollapseAll();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxTreeView);
};
