#include "KxStdAfx.h"
#include "HeaderCtrl.h"
#include "View.h"
#include "MainWindow.h"
#include "Column.h"
#include "KxFramework/KxDataView2Event.h"

namespace KxDataView2
{
	void HeaderCtrl::FinishEditing()
	{
		m_View->GetMainWindow()->EndEdit();
	}
	bool HeaderCtrl::SendEvent(wxEventType type, int index)
	{
		Event event(type);
		m_View->GetMainWindow()->CreateEventTemplate(event, nullptr, index >= 0 ? m_View->GetColumn(index) : nullptr);
		return m_View->ProcessWindowEvent(event);
	}

	void HeaderCtrl::OnClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		const int columnIndex = event.GetColumn();
		if (SendEvent(KxEVT_DATAVIEW_COLUMN_HEADER_CLICK, columnIndex))
		{
			return;
		}

		// Default handling for the column click is to sort by this column or  toggle its sort order
		Column* column = m_View->GetColumn(columnIndex);
		if (!column->IsSortable())
		{
			// No default handling for non-sortable columns
			event.Skip();
			return;
		}

		if (column->IsSorted())
		{
			// already using this column for sorting, just change the order
			column->ToggleSortOrder();
		}
		else
		{
			// Not using this column for sorting yet.
			// We will sort by this column only now, so reset all the previously used ones.
			m_View->ResetAllSortColumns();
			column->SortAscending();
		}

		m_View->GetMainWindow()->OnShouldResort();
		m_View->OnColumnChange(columnIndex);
		SendEvent(KxEVT_DATAVIEW_COLUMN_SORTED, columnIndex);
	}
	void HeaderCtrl::OnRClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		// Event wasn't processed somewhere, use default behavior
		if (!SendEvent(KxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, event.GetColumn()))
		{
			event.Skip();
			ToggleSortByColumn(event.GetColumn());
		}
	}
	void HeaderCtrl::OnResize(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		int index = event.GetColumn();
		int width = event.GetWidth();
		Column* column = m_View->GetColumn(index);

		column->SetWidth(width);
		m_View->OnColumnChange(index);
	}
	void HeaderCtrl::OnEndReorder(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		int index = event.GetColumn();
		m_View->ColumnMoved(*m_View->GetColumn(index), event.GetNewOrder());
	}
	void HeaderCtrl::OnWindowClick(wxMouseEvent& event)
	{
		wxPoint pos = event.GetPosition();
		if (pos.x > m_View->GetMainWindow()->GetRowWidth())
		{
			SendEvent(event.GetEventType() == wxEVT_LEFT_UP ? KxEVT_DATAVIEW_COLUMN_HEADER_CLICK : KxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, -1);
		}
		event.Skip();
	}

	const wxHeaderColumn& HeaderCtrl::GetColumn(unsigned int index) const
	{
		return m_View->GetColumn(index)->GetNativeColumn();
	}
	bool HeaderCtrl::UpdateColumnWidthToFit(unsigned int index, int titleWidth)
	{
		Column* column = m_View->GetColumn(index);

		int contentsWidth = column->CalcBestSize();
		column->SetWidth(std::max({titleWidth, contentsWidth, column->GetMinWidth()}));
		m_View->OnColumnChange(index);
		return true;
	}

	HeaderCtrl::HeaderCtrl(View* parent)
		:wxHeaderCtrl(parent), m_View(parent)
	{
		Bind(wxEVT_HEADER_CLICK, &HeaderCtrl::OnClick, this);
		Bind(wxEVT_HEADER_RIGHT_CLICK, &HeaderCtrl::OnRClick, this);
		Bind(wxEVT_HEADER_RESIZING, &HeaderCtrl::OnResize, this);
		Bind(wxEVT_HEADER_END_RESIZE, &HeaderCtrl::OnResize, this);
		Bind(wxEVT_HEADER_END_REORDER, &HeaderCtrl::OnEndReorder, this);

		Bind(wxEVT_LEFT_UP, &HeaderCtrl::OnWindowClick, this);
		Bind(wxEVT_RIGHT_UP, &HeaderCtrl::OnWindowClick, this);
	}

	void HeaderCtrl::ToggleSortByColumn(size_t columnIndex)
	{
		if (!m_View->IsMultiColumnSortAllowed())
		{
			return;
		}

		Column* column = m_View->GetColumn(columnIndex);
		if (column->IsSortable())
		{
			if (column->IsSorted())
			{
				column->ResetSorting();
				SendEvent(KxEVT_DATAVIEW_COLUMN_SORTED, columnIndex);
			}
			else
			{
				// Start sorting by it.
				column->SortAscending();
				SendEvent(KxEVT_DATAVIEW_COLUMN_SORTED, columnIndex);
			}
		}
	}
}
