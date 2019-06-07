#include "KxStdAfx.h"
#include "HeaderCtrl.h"
#include "View.h"
#include "MainWindow.h"
#include "Column.h"
#include "KxFramework/KxDataView2Event.h"
#include <CommCtrl.h>

namespace KxDataView2
{
	void HeaderCtrl::FinishEditing()
	{
		m_View->GetMainWindow()->EndEdit();
	}
	bool HeaderCtrl::SendEvent(wxEventType type, int index, std::optional<wxRect> rect)
	{
		Event event(type);
		if (rect)
		{
			event.SetRect(*rect);
		}
		m_View->GetMainWindow()->CreateEventTemplate(event, nullptr, index >= 0 ? GetColumnAt(index) : nullptr);

		return m_View->ProcessWindowEvent(event);
	}
	
	int HeaderCtrl::ToNativeColumnIndex(size_t index) const
	{
		// Copied from private function wxHeaderCtrl::MSWToNativeIdx

		// Don't check for GetColumn(idx).IsShown() as it could have just became
		// false and we may be called from DoUpdate() to delete the old column

		// I can't use this assert here, no access to 'm_isHidden'
		// wxASSERT_MSG(!m_isHidden[index], "column must be visible to have an index in the native control");

		int nativeIndex = index;
		for (size_t i = 0; i < index; i++)
		{
			if (GetColumn(i).IsHidden())
			{
				// One less column the native control knows about
				nativeIndex--;
			}
		}

		wxASSERT_MSG(nativeIndex >= 0 && nativeIndex <= GetShownColumnsCount(), "logic error");
		return nativeIndex;
	}
	size_t HeaderCtrl::FromNativeColumnIndex(int nativeIndex) const
	{
		// Copied from 'wxHeaderCtrl::MSWFromNativeIdx'
		wxASSERT_MSG(nativeIndex >= 0 && nativeIndex < GetShownColumnsCount(), "column index out of range");

		// Reverse the above function
		size_t index = nativeIndex;
		for (size_t i = 0; i < wxHeaderCtrl::GetColumnCount(); i++)
		{
			if (i > index)
			{
				break;
			}
			if (GetColumn(i).IsHidden())
			{
				index++;
			}
		}

		wxASSERT_MSG(ToNativeColumnIndex(index) == nativeIndex, "logic error");
		return index;
	}

	void HeaderCtrl::OnClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		const int columnIndex = event.GetColumn();
		if (SendEvent(EVENT_COLUMN_HEADER_CLICK, columnIndex))
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
		SendEvent(EVENT_COLUMN_SORTED, columnIndex);
	}
	void HeaderCtrl::OnRClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		// Event wasn't processed somewhere, use default behavior
		if (!SendEvent(EVENT_COLUMN_HEADER_RCLICK, event.GetColumn()))
		{
			event.Skip();
			ToggleSortByColumn(event.GetColumn());
		}
	}
	void HeaderCtrl::OnResize(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		const int index = event.GetColumn();
		const int width = event.GetWidth();
		Column* column = m_View->GetColumn(index);

		column->SetWidth(width);
		m_View->OnColumnChange(index);
	}
	void HeaderCtrl::OnReordered(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		const int index = event.GetColumn();
		m_View->ColumnMoved(*m_View->GetColumn(index), event.GetNewOrder());
	}
	void HeaderCtrl::OnWindowClick(wxMouseEvent& event)
	{
		wxPoint pos = event.GetPosition();
		if (pos.x > m_View->GetMainWindow()->GetRowWidth())
		{
			SendEvent(event.GetEventType() == wxEVT_LEFT_UP ? EVENT_COLUMN_HEADER_CLICK : EVENT_COLUMN_HEADER_RCLICK, -1);
		}
		event.Skip();
	}
	
	bool HeaderCtrl::UpdateColumnWidthToFit(unsigned int index, int titleWidth)
	{
		Column* column = m_View->GetColumn(index);

		int contentsWidth = column->CalcBestSize();
		column->SetWidth(std::max({titleWidth, contentsWidth, column->GetMinWidth()}));
		m_View->OnColumnChange(index);
		return true;
	}
	const wxHeaderColumn& HeaderCtrl::GetColumn(unsigned int index) const
	{
		return GetColumnAt(index)->GetNativeColumn();
	}
	bool HeaderCtrl::MSWHandleNotify(WXLRESULT* result, int notification, WXWPARAM wParam, WXLPARAM lParam)
	{
		const NMHEADERW* header = reinterpret_cast<NMHEADERW*>(lParam);
		switch (notification)
		{
			// The control doesn't display drag image on reordering columns if double-buffering
			// is enabled. That looks really bad so we'll temporarily disable double buffering
			// for the duration of the drag and drop operation.
			case (int)HDN_BEGINDRAG:
			{
				SetDoubleBuffered(false);
				return true;
			}
			case (int)HDN_ENDDRAG:
			{
				SetDoubleBuffered(true);
				return true;
			}

			case (int)HDN_ITEMSTATEICONCLICK:
			{
				Column* column = GetColumnAt(FromNativeColumnIndex(header->iItem));
				if (column)
				{
					// Send an event, if it wasn't processed, toggle check state ourselves.
					// In any case update native column state after the event handler returns.
					const bool isChecked = column->IsChecked();
					if (!SendEvent(EVENT_COLUMN_TOGGLE, column->GetIndex()))
					{
						column->SetChecked(!isChecked);
					}
					UpdateColumn(*column);
				}
				return true;
			}
			case (int)HDN_DROPDOWN:
			{
				RECT winRect = {};
				Header_GetItemDropDownRect(GetHandle(), header->iItem, &winRect);

				// MSDN says "the coordinates returned in the RECT structure are expressed as screen coordinates",
				// but in fact they are in parent window client coordinates, so no additional translation needed.
				SendEvent(EVENT_COLUMN_DROPDOWN, FromNativeColumnIndex(header->iItem), KxUtility::CopyRECTToRect(winRect));
				return true;
			}
		};
		return false;
	}

	void HeaderCtrl::OnColumnInserted(Column& column)
	{
		//UpdateColumn(column);
	}
	void HeaderCtrl::UpdateColumn(Column& column)
	{
		if (column.IsVisible())
		{
			wxHeaderCtrl::UpdateColumn(column.GetIndex());
			const HWND hwnd = GetHandle();
			const int nativeIndex = ToNativeColumnIndex(column.GetIndex());

			// Get current format
			HDITEMW item = {0};
			item.mask = HDI_FORMAT|HDI_STATE;
			Header_GetItem(hwnd, nativeIndex, &item);

			// Dropdown
			KxUtility::ModFlagRef(item.fmt, HDF_SPLITBUTTON, column.HasDropDown());

			// Checkbox
			KxUtility::ModFlagRef(item.fmt, HDF_CHECKBOX, column.GetStyle().IsEnabled(ColumnStyle::CheckBox));
			KxUtility::ModFlagRef(item.fmt, HDF_CHECKED, column.IsChecked());

			// Update the column format
			item.state = HDIS_FOCUSED;
			Header_SetItem(hwnd, nativeIndex, &item);
		}
	}
	void HeaderCtrl::UpdateColumn(size_t index)
	{
		if (Column* column = GetColumnAt(index))
		{
			UpdateColumn(*column);
		}
	}
	void HeaderCtrl::UpdateDisplay()
	{
		for (auto& column: m_View->m_Columns)
		{
			UpdateColumn(*column);
		}
	}

	HeaderCtrl::HeaderCtrl(View* parent)
		:wxHeaderCtrl(parent), m_View(parent)
	{
		// General
		KxUtility::ToggleWindowStyle(GetHandle(), GWL_STYLE, HDS_CHECKBOXES, true);
		SetBackgroundColour(m_View->GetBackgroundColour());
		SetDoubleBuffered(true);

		// Events
		Bind(wxEVT_HEADER_CLICK, &HeaderCtrl::OnClick, this);
		Bind(wxEVT_HEADER_RIGHT_CLICK, &HeaderCtrl::OnRClick, this);
		Bind(wxEVT_HEADER_RESIZING, &HeaderCtrl::OnResize, this);
		Bind(wxEVT_HEADER_END_RESIZE, &HeaderCtrl::OnResize, this);
		Bind(wxEVT_HEADER_END_REORDER, &HeaderCtrl::OnReordered, this);

		Bind(wxEVT_LEFT_UP, &HeaderCtrl::OnWindowClick, this);
		Bind(wxEVT_RIGHT_UP, &HeaderCtrl::OnWindowClick, this);
	}

	size_t HeaderCtrl::GetColumnCount() const
	{
		return m_View->GetColumnCount();
	}
	Column* HeaderCtrl::GetColumnAt(size_t index) const
	{
		return m_View->GetColumn(index);
	}
	Column* HeaderCtrl::GetColumnDisplayedAt(size_t index) const
	{
		return m_View->GetColumnDisplayedAt(index);
	}

	void HeaderCtrl::ToggleSortByColumn(size_t index)
	{
		if (m_View->IsMultiColumnSortAllowed())
		{
			return;
		}
		else if (Column* column = m_View->GetColumn(index))
		{
			ToggleSortByColumn(*column);
		}
	}
	void HeaderCtrl::ToggleSortByColumn(Column& column)
	{
		if (m_View->IsMultiColumnSortAllowed() && column.IsSortable())
		{
			if (column.IsSorted())
			{
				column.ResetSorting();
			}
			else
			{
				column.SortAscending();
			}
			SendEvent(EVENT_COLUMN_SORTED, column.GetIndex());
		}
	}
}
