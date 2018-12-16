#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxListView.h"

class KX_API KxListBox: public KxListView
{
	private:
		void OnSelect(wxListEvent& event);
		void OnDoubleClick(wxListEvent& event);
		void OnCheck(wxListEvent& event);
		void OnInsertItem(wxListEvent& event);
		void OnItemMenu(wxListEvent& event);
		void OnSize(wxSizeEvent& event);

		// Sorting can only use ItemData, so custom sorting function needed
		static int wxCALLBACK SortComparator(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);
		void OnSortNeeded(wxListEvent& event);

	public:
		static const long DefaultStyle = KxLV_REPORT|KxLV_AUTOARRANGE|KxLV_NO_HEADER|KxLV_SINGLE_SEL;

		KxListBox() {}
		KxListBox(wxWindow* parent,
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

	public:
		void SetItemLabel(int index, const wxString& label)
		{
			SetItemText(index, label);
		}
		bool IsCheckList()
		{
			return HasCheckboxes();
		}
		void SetCheckList(bool value)
		{
			EnableCheckboxes(value);
		}
		
		int InsertItem(const wxString& label, size_t index, int imageID = wxWithImages::NO_IMAGE)
		{
			return KxListView::InsertItem(label, index, imageID);
		}
		int AddItem(const wxString& label, int imageID = wxWithImages::NO_IMAGE)
		{
			return KxListView::InsertItem(label, GetItemCount() + 1, imageID);
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxListBox);
};
