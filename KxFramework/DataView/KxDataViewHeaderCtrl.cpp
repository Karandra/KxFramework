#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewHeaderCtrl.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/KxDataViewEvent.h"

void KxDataViewHeaderCtrl::FinishEditing()
{
	GetOwner()->GetMainWindow()->EndEdit();
}
bool KxDataViewHeaderCtrl::SendEvent(wxEventType type, int index)
{
	KxDataViewEvent event(type);
	
	KxDataViewCtrl* owner = GetOwner();
	owner->GetMainWindow()->CreateEventTemplate(event, KxDataViewItem(), index >= 0 ? owner->GetColumn(index) : NULL, true);

	// For events created by 'KxDataViewHeaderCtrl' the row/value fields are not valid.
	return owner->ProcessWindowEvent(event);
}

void KxDataViewHeaderCtrl::OnClick(wxHeaderCtrlEvent& event)
{
	FinishEditing();

	const int columnIndex = event.GetColumn();
	if (SendEvent(KxEVT_DATAVIEW_COLUMN_HEADER_CLICK, columnIndex))
	{
		return;
	}

	// Default handling for the column click is to sort by this column or  toggle its sort order
	KxDataViewCtrl* owner = GetOwner();
	KxDataViewColumn* column = owner->GetColumn(columnIndex);
	if (!column->IsSortable())
	{
		// No default handling for non-sortable columns
		event.Skip();
		return;
	}

	if (column->IsSortKey())
	{
		// already using this column for sorting, just change the order
		column->ToggleSortOrder();
	}
	else
	{
		// Not using this column for sorting yet

		// We will sort by this column only now, so reset all the
		// previously used ones.
		owner->ResetAllSortColumns();

		// Sort the column
		column->SortAscending();
	}

	KxDataViewModel* model = owner->GetModel();
	if (model)
	{
		model->Resort();
	}
	owner->OnColumnChange(columnIndex);
	SendEvent(KxEVT_DATAVIEW_COLUMN_SORTED, columnIndex);
}
void KxDataViewHeaderCtrl::OnRClick(wxHeaderCtrlEvent& event)
{
	FinishEditing();

	// Event wasn't processed somewhere, use default behavior
	if (!SendEvent(KxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, event.GetColumn()))
	{
		event.Skip();
		ToggleSortByColumn(event.GetColumn());
	}
}
void KxDataViewHeaderCtrl::OnResize(wxHeaderCtrlEvent& event)
{
	FinishEditing();
	KxDataViewCtrl* owner = GetOwner();

	int index = event.GetColumn();
	owner->GetColumn(index)->SetWidth(event.GetWidth());
	owner->OnColumnChange(index);
}
void KxDataViewHeaderCtrl::OnEndReorder(wxHeaderCtrlEvent& event)
{
	FinishEditing();
	KxDataViewCtrl* owner = GetOwner();

	int index = event.GetColumn();
	owner->ColumnMoved(owner->GetColumn(index), event.GetNewOrder());
}

void KxDataViewHeaderCtrl::OnWindowClick(wxMouseEvent& event)
{
	wxPoint pos = event.GetPosition();
	if (pos.x > GetOwner()->GetMainWindow()->GetEndOfLastCol())
	{
		SendEvent(event.GetEventType() == wxEVT_LEFT_UP ? KxEVT_DATAVIEW_COLUMN_HEADER_CLICK : KxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, -1);
	}
	event.Skip();
}

const wxHeaderColumn& KxDataViewHeaderCtrl::GetColumn(unsigned int index) const
{
	return *GetOwner()->GetColumn(index)->GetAsSettableHeaderColumn();
}
bool KxDataViewHeaderCtrl::UpdateColumnWidthToFit(unsigned int index, int titleWidth)
{
	KxDataViewCtrl* owner = GetOwner();
	KxDataViewColumn* column = owner->GetColumn(index);

	int contentsWidth = owner->GetBestColumnWidth(index);
	column->SetWidth(std::max({titleWidth, contentsWidth, column->GetMinWidth()}));
	owner->OnColumnChange(index);
	return true;
}

KxDataViewHeaderCtrl::KxDataViewHeaderCtrl(KxDataViewCtrl* parent)
	:wxHeaderCtrl(parent)
{
	Bind(wxEVT_HEADER_CLICK, &KxDataViewHeaderCtrl::OnClick, this);
	Bind(wxEVT_HEADER_RIGHT_CLICK, &KxDataViewHeaderCtrl::OnRClick, this);
	Bind(wxEVT_HEADER_RESIZING, &KxDataViewHeaderCtrl::OnResize, this);
	Bind(wxEVT_HEADER_END_RESIZE, &KxDataViewHeaderCtrl::OnResize, this);
	Bind(wxEVT_HEADER_END_REORDER, &KxDataViewHeaderCtrl::OnEndReorder, this);

	Bind(wxEVT_LEFT_UP, &KxDataViewHeaderCtrl::OnWindowClick, this);
	Bind(wxEVT_RIGHT_UP, &KxDataViewHeaderCtrl::OnWindowClick, this);
}

KxDataViewCtrl* KxDataViewHeaderCtrl::GetOwner()
{
	return static_cast<KxDataViewCtrl*>(GetParent());
}
const KxDataViewCtrl* KxDataViewHeaderCtrl::GetOwner() const
{
	return static_cast<KxDataViewCtrl*>(GetParent());
}

void KxDataViewHeaderCtrl::ToggleSortByColumn(size_t columnIndex)
{
	KxDataViewCtrl* owner = GetOwner();
	if (!owner->IsMultiColumnSortAllowed())
	{
		return;
	}

	KxDataViewColumn* column = owner->GetColumn(columnIndex);
	if (column->IsSortable())
	{
		if (owner->IsColumnSorted(columnIndex))
		{
			column->ResetSorting();
			SendEvent(KxEVT_DATAVIEW_COLUMN_SORTED, columnIndex);
		}
		else
		{
			// Do start sorting by it.
			column->SortAscending();
			SendEvent(KxEVT_DATAVIEW_COLUMN_SORTED, columnIndex);
		}
	}
}
