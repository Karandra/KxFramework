#pragma once
#include "KxFramework/KxFramework.h"

enum
{
	KxLV_VRULES = wxLC_VRULES,
	KxLV_HRULES = wxLC_HRULES,

	KxLV_ICON = wxLC_ICON,
	KxLV_SMALL_ICON = wxLC_SMALL_ICON,
	KxLV_LIST = wxLC_LIST,
	KxLV_REPORT = wxLC_REPORT,
	KxLV_TILE = wxLC_SORT_DESCENDING << 1,

	KxLV_ALIGN_TOP = wxLC_ALIGN_TOP,
	KxLV_ALIGN_LEFT = wxLC_ALIGN_LEFT,
	KxLV_AUTOARRANGE = wxLC_AUTOARRANGE,
	KxLV_VIRTUAL = wxLC_VIRTUAL,
	KxLV_EDIT_LABELS = wxLC_EDIT_LABELS,
	KxLV_NO_HEADER = wxLC_NO_HEADER,
	KxLV_NO_SORT_HEADER = wxLC_NO_SORT_HEADER,
	KxLV_SINGLE_SEL = wxLC_SINGLE_SEL,
	KxLV_SORT_ASCENDING = wxLC_SORT_ASCENDING,
	KxLV_SORT_DESCENDING = wxLC_SORT_DESCENDING,

	KxLV_MASK_TYPE = wxLC_MASK_TYPE,
	KxLV_MASK_ALIGN = wxLC_MASK_ALIGN,
	KxLV_MASK_SORT = wxLC_MASK_SORT,

	KxLV_INVALID_WIDTH = -100,
};

class KX_API KxListView: public wxListView
{
	private:
		int m_ItemHeight = 21;

		int m_TileViewWidth = -1;
		int m_TileViewHeight = -1;
		bool m_TileViewAutoSize = true;
		bool m_InTileView = false;

	private:
		void UpdateTileView();
		void UpdateTileViewItem(wxListEvent& event);
		void OnPaint(wxPaintEvent& event);

	public:
		enum SortArrow
		{
			ARROW_NONE,
			ARROW_UP,
			ARROW_DOWN
		};

		static const long DefaultStyle = KxLV_REPORT|KxLV_AUTOARRANGE|KxLV_SINGLE_SEL;
		static const int DefaultItemMask = wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE|wxLIST_MASK_DATA|wxLIST_MASK_WIDTH|wxLIST_MASK_FORMAT;

		KxListView() {}
		KxListView(wxWindow* parent,
				   wxWindowID winid,
				   long style = DefaultStyle
		)
		{
			Create(parent, winid, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID winid,
					long style = DefaultStyle
		);
		virtual ~KxListView();
		
	public:
		int GetViewMode();
		void SetViewMode(int viewMode);

		bool IsMultiSelect();
		void SetMultiSelect(bool value);
	
		// Columns
		SortArrow GetSortArrow(int index) const;
		void SetSortArrow(int index, SortArrow mode);

		int InsertColumn(const wxString& label, int width, size_t index, int imageID = wxWithImages::NO_IMAGE, wxListColumnFormat alignment = wxLIST_FORMAT_LEFT);
		int AddColumn(const wxString& label, int width, int imageID = wxWithImages::NO_IMAGE, wxListColumnFormat alignment = wxLIST_FORMAT_LEFT);
		void RemoveColumn(int i);
		void ClearColumns();
		void SelectColumn(int i);
		wxString GetColumnLabel(int i) const;
		void SetColumnLabel(int i, const wxString& label);
		int GetColumnWidth(int i) const;
		bool SetColumnWidth(int i, int width = wxLIST_AUTOSIZE);
		int GetColumnImage(int i) const;
		void SetColumnImage(int i, int imageID = wxWithImages::NO_IMAGE);
		wxListColumnFormat GetColumnAlignment(int i) const;
		void SetColumnAlignment(int i, wxListColumnFormat imageID = wxLIST_FORMAT_LEFT);

		// Items
		void ClearItems();
		void RemoveItem(int i);
		int GetSelection() const;
		std::vector<int> GetSelections() const;
		std::vector<int> GetCheckedItems() const;
		int InsertItem(const wxString& label, size_t index, int imageID = wxWithImages::NO_IMAGE);
		int InsertItem(const std::vector<wxString>& labels, size_t index, int imageID = wxWithImages::NO_IMAGE);
		int AddItem(const wxString& label, int imageID = wxWithImages::NO_IMAGE);
		int AddItem(const std::vector<wxString>& labels, int imageID = wxWithImages::NO_IMAGE);
		bool IsItemSelected(int i) const;
		void FocusItem(int i);
		void SetAllItemSelected(bool b);
		void SelectItem(int i, bool b);
		void SetAllItemChecked(bool b);
		void SetHotItem(int i);

		wxString GetItemLabel(int row, int columnIndex) const;
		std::vector<wxString> GetItemLabels(int row) const;
		void SetItemLabel(const wxString& label, int row, int columnIndex);
		void SetItemLabels(const std::vector<wxString>& labels, int row);
		int GetItemImage(int row, int columnIndex) const;
		void SetItemImage(int row, int columnIndex, int imageID = wxWithImages::NO_IMAGE);
		void SetItemChecked(int row, bool b);
		int GetItemState(int row) const;
		void SetItemState(int row, int state, bool set = true);
		wxFont GetItemFont(int row, int columnIndex) const;
		void SetItemFont(int row, int columnIndex, const wxFont& v);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxListView);
};
