#include "KxStdAfx.h"
#include "KxFramework/KxListBox.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxListBox, KxListView);

int wxCALLBACK KxListBox::SortComparator(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	return 0;
}
void KxListBox::OnSortNeeded(wxListEvent& event)
{
	SortItems(SortComparator, (wxIntPtr)this);
}

void KxListBox::OnSelect(wxListEvent& event)
{
	bool isSelected = event.GetEventType() == wxEVT_LIST_ITEM_SELECTED;

	wxCommandEvent command(wxEVT_LISTBOX, GetId());
	command.SetInt(isSelected ? event.GetIndex() : -1);
	command.SetExtraLong(isSelected);

	HandleWindowEvent(command);
}
void KxListBox::OnDoubleClick(wxListEvent& event)
{
	wxCommandEvent command(wxEVT_LISTBOX_DCLICK, GetId());
	command.SetInt(event.GetIndex());

	HandleWindowEvent(command);
}
void KxListBox::OnCheck(wxListEvent& event)
{
	wxCommandEvent command(wxEVT_CHECKLISTBOX, GetId());
	command.SetInt(KxListView::IsItemChecked(event.GetIndex()));
	command.SetExtraLong(event.GetIndex());

	HandleWindowEvent(command);
}
void KxListBox::OnInsertItem(wxListEvent& event)
{
	if (GetItemCount() < GetCountPerPage())
	{
		SetColumnWidth(0, GetSize().GetWidth()-4);
	}
	else
	{
		SetColumnWidth(0, GetSize().GetWidth()-18);
	}
	Refresh();
}
void KxListBox::OnItemMenu(wxListEvent& event)
{
	wxContextMenuEvent command(wxEVT_CONTEXT_MENU, GetId());
	command.SetInt(event.GetIndex());
	command.SetPosition(event.GetPoint());

	HandleWindowEvent(command);
	event.Skip();
}
void KxListBox::OnSize(wxSizeEvent& event)
{
	wxListEvent listEvent;
	OnInsertItem(listEvent);
}

bool KxListBox::Create(wxWindow* parent,
					   wxWindowID winid,
					   long style
)
{
	if (KxListView::Create(parent, winid, style))
	{
		SetSingleStyle(wxLC_VRULES, false);
		AddColumn(wxEmptyString, wxLIST_AUTOSIZE, -1);
		//SetColumnWidth(0, GetColumnWidth(0)-4);

		Bind(wxEVT_LIST_ITEM_SELECTED, &KxListBox::OnSelect, this);
		Bind(wxEVT_LIST_ITEM_DESELECTED, &KxListBox::OnSelect, this);
		Bind(wxEVT_LIST_ITEM_ACTIVATED, &KxListBox::OnDoubleClick, this);
		Bind(wxEVT_LIST_ITEM_CHECKED, &KxListBox::OnCheck, this);
		Bind(wxEVT_LIST_ITEM_UNCHECKED, &KxListBox::OnCheck, this);
		Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &KxListBox::OnItemMenu, this);

		Bind(wxEVT_SIZE, &KxListBox::OnSize, this);
		Bind(wxEVT_LIST_INSERT_ITEM, &KxListBox::OnInsertItem, this);
		Bind(wxEVT_LIST_DELETE_ITEM, &KxListBox::OnInsertItem, this);
		return true;
	}
	return false;
}
