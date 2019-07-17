#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithImageList.h"
#include "KxEvent.h"

KxEVENT_DECLARE_GLOBAL(TREELIST_ITEM_ACTIVATED, wxTreeListEvent);
KxEVENT_DECLARE_GLOBAL(TREELIST_SELECTION_CHANGED, wxTreeListEvent);

enum KxTreeList_ImageState
{
	KxTL_IMAGE_COLLAPSED,
	KxTL_IMAGE_EXPANDED
};
enum KxTreeListOptions
{
	KxTL_NONE = 0,
	KxTL_FIRST = wxTL_STYLE_MASK + 1,

	KxTL_VERT_RULES = KxTL_FIRST << 1,
	KxTL_HORIZ_RULES = KxTL_FIRST << 2,
	KxTL_DCLICK_EXPAND = KxTL_FIRST << 3,
	KxTL_FIX_FIRST_COLUMN = KxTL_FIRST << 4,

	KxTL_MASK = KxTL_VERT_RULES|KxTL_HORIZ_RULES|KxTL_DCLICK_EXPAND|KxTL_FIX_FIRST_COLUMN
};

class KX_API [[deprecated("Use KxDataView2 with custom data model")]] KxTreeList;
class KX_API [[deprecated("Use KxDataView2 with custom data model")]] KxTreeListItem;

class KX_API KxTreeList;
class KX_API KxTreeListItem
{
	private:
		KxTreeList* m_Control = nullptr;
		wxTreeListItem m_Item;

	private:
		void OnCreate();
		KxTreeListItem(const wxTreeListItem& v)
			:m_Item(v)
		{

		}

	public:
		enum 
		{
			NO_IMAGE = wxWithImages::NO_IMAGE,
			DONT_CHANGE = NO_IMAGE - 1,
			SAME = NO_IMAGE - 2,
		};
		static const KxTreeListItem First;
		static const KxTreeListItem Last;

		KxTreeListItem() {}
		KxTreeListItem(KxTreeList& control, const wxTreeListItem& v);
		void Create(KxTreeList& control, const wxTreeListItem& v);
		~KxTreeListItem() {}

	public:
		bool IsOK() const
		{
			return m_Item.IsOk();
		}
		KxTreeList& GetControl()
		{
			return *m_Control;
		}

		KxTreeListItem Add(const wxString& label, wxClientData* data = nullptr)
		{
			return Insert(Last, label, data);
		}
		KxTreeListItem Add(const KxStringVector& labels, wxClientData* data = nullptr)
		{
			return Insert(Last, labels, data);
		}
		KxTreeListItem Insert(const KxTreeListItem& previous = First, const wxString& label = wxEmptyString, wxClientData* data = nullptr);
		KxTreeListItem Insert(const KxTreeListItem& previous, const KxStringVector& labels, wxClientData* data = nullptr);

		KxTreeListItem GetFirstChild() const;
		KxTreeListItem GetNthChild(size_t index) const;
		KxTreeListItem GetNext() const;
		KxTreeListItem GetPrevSibling() const;
		KxTreeListItem GetNextSibling() const;
		KxTreeListItem GetParent() const;

		void Remove();
		void RemoveChildren();
		void SetChildrenExpanded(bool b = true);
		void ExpandChildren()
		{
			SetChildrenExpanded(true);
		}
		void CollapseChildren()
		{
			SetChildrenExpanded(false);
		}
		void Expand()
		{
			SetExpanded(true);
		}
		void Collape()
		{
			SetExpanded(false);
		}
		bool IsExpanded() const;
		void SetExpanded(bool expanded);
		void EnsureVisible();
		void SetFocus();
		bool IsSelected() const;
		void SetSelection(bool b = true);
		bool HasChildren() const
		{
			return GetFirstChild().IsOK();
		}
		int GetIndexWithinParent() const;

		KxStringVector GetLabels() const;
		void SetLabels(const KxStringVector& labels);

		wxString GetLabel(size_t i = 0) const;
		void SetLabel(const wxString& s, size_t i = 0);

		void BeginEditLabel(size_t i = 0);
		void EndEditLabel(bool discardChnages = false, size_t i = 0);

		int GetImage(KxTreeList_ImageState type) const;
		void SetImage(int collapsed, int expanded = SAME);

		wxClientData* GetData() const;
		void SetData(wxClientData* p);

		bool IsChecked() const;
		void SetChecked(bool b);

		wxCheckBoxState GetChecked() const;
		void SetChecked(wxCheckBoxState v);

		void Swap(KxTreeListItem& other);
		wxRect GetRect() const;

	public:
		operator const wxTreeListItem() const
		{
			return m_Item;
		}
		operator wxTreeListItem& ()
		{
			return m_Item;
		}
};
typedef std::vector<KxTreeListItem> KxTreeListItems;

//////////////////////////////////////////////////////////////////////////
class KX_API KxTreeList: public KxWithImageListWrapper<wxTreeListCtrl>
{
	friend class KxTreeListItem;

	private:
		wxDataViewCtrl* m_DataView = nullptr;
		wxHeaderCtrl* m_HeaderCtrl = nullptr;
		KxTreeListOptions m_Options = KxTL_NONE;
		int m_RowHeight = 0;
		int m_FirstColumnWidth = wxCOL_WIDTH_AUTOSIZE;

	private:
		wxTreeListModelNode* GetItemNode(const wxTreeListItem& item) const;
		wxDataViewRenderer* GetColumnRendererDV(size_t index) const;
		wxTreeListItem DVItemToTL(const wxDataViewItem& item) const;
		void InitHeaderWindow();
		void SetDoubleClickExpandEnabled(bool value);

		void OnResize(wxSizeEvent& event);
		void OnHeaderResized(wxHeaderCtrlEvent& event);
		void EvtExpandOnDClick(wxTreeListEvent& event);
		void OnSelectDV(wxDataViewEvent& event);
		void OnActivateDV(wxDataViewEvent& event);

		wxTreeListItem InsertItem(const wxTreeListItem& parentItem, const wxTreeListItem& prevItem, const wxString& label, int imageCollapsed = NO_IMAGE, int imageExpanded = NO_IMAGE, wxClientData* data = nullptr)
		{
			return wxTreeListCtrl::InsertItem(parentItem, prevItem, label, imageCollapsed, imageExpanded, data);
		}
		wxTreeListItem AppendItem(const wxTreeListItem& parentItem, const wxString& label, int imageCollapsed = NO_IMAGE, int imageExpanded = NO_IMAGE, wxClientData* data = nullptr)
		{
			return wxTreeListCtrl::AppendItem(parentItem, label, imageCollapsed, imageExpanded, data);
		}

		wxRect GetItemRect(const wxDataViewItem& item, size_t columnIndex = -1) const;
		wxRect GetItemRect(const wxTreeListItem& item, size_t columnIndex = -1) const;
		wxRect GetHeaderRect() const;
		wxTreeListItem GetPrevSibling(const wxTreeListItem& item) const;

		int GetItemImage(const wxTreeListItem& item, int mode = KxTL_IMAGE_COLLAPSED);
		void SetItemImage(const wxTreeListItem& item, int nImage, int mode = KxTL_IMAGE_COLLAPSED);

	public:
		static const long DefaultStyle = wxTL_DEFAULT_STYLE;
		static const int DefaultIndent = 20;
		static const int DefaultRowHeight = 22;

		KxTreeList() {}
		KxTreeList(wxWindow* parent,
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

	public:
		wxDataViewItem GetItemDV(const wxTreeListItem& item) const;
		wxDataViewColumn* GetColumnDV(size_t index) const;
		wxHeaderCtrl* GetHeader() const
		{
			return m_HeaderCtrl;
		}

		// Columns
		int AddColumn(const wxString& label, int width = wxCOL_WIDTH_AUTOSIZE, wxAlignment align = wxALIGN_LEFT, int flags = wxCOL_RESIZABLE);
		void SetColumnDisplayOrder(int oldIndex, int newIndex);
		void ClearItems()
		{
			DeleteAllItems();
		}

		KxTreeListItem GetRoot() const
		{
			return KxTreeListItem(*const_cast<KxTreeList*>(this), wxTreeListCtrl::GetRootItem());
		}
		KxTreeListItem GetFirstItem() const
		{
			return KxTreeListItem(*const_cast<KxTreeList*>(this), wxTreeListCtrl::GetFirstItem());
		}

		KxTreeListItem GetSelection() const;
		KxTreeListItems GetSelections() const;

		void SetColumnWidth(unsigned col, int width);
		int GetIndent() const
		{
			return m_DataView->GetIndent();
		}
		void SetIndent(int value)
		{
			m_DataView->SetIndent(value);
		}
		int GetRowHeight() const
		{
			return m_RowHeight;
		}
		void SetRowHeight(int value)
		{
			const int nMinHeight = 17;
			if (value < nMinHeight)
			{
				value = nMinHeight;
			}
			m_RowHeight = value;
			m_DataView->SetRowHeight(value);
			Refresh();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxTreeList);
};
