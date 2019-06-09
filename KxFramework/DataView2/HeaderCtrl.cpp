#include "KxStdAfx.h"
#include "HeaderCtrl.h"
#include "View.h"
#include "MainWindow.h"
#include "Column.h"
#include "KxFramework/KxDataView2Event.h"
#include <CommCtrl.h>

namespace
{
	class wxHeaderCtrlClone: public wxHeaderCtrlBase
	{
		public:
			wxHeaderCtrlClone();
			wxHeaderCtrlClone(wxWindow* parent,
							  wxWindowID id = wxID_ANY,
							  const wxPoint& pos = wxDefaultPosition,
							  const wxSize& size = wxDefaultSize,
							  long style = wxHD_DEFAULT_STYLE,
							  const wxString& name = wxHeaderCtrlNameStr);
			bool Create(wxWindow* parent,
						wxWindowID id = wxID_ANY,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						long style = wxHD_DEFAULT_STYLE,
						const wxString& name = wxHeaderCtrlNameStr);
			virtual ~wxHeaderCtrlClone();

		public:
			bool SetBackgroundColour(const wxColour& colour) override;
			bool SetForegroundColour(const wxColour& colour) override;
			bool SetFont(const wxFont& font) override;

		public:
			wxSize DoGetBestSize() const override;
			void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) override;

		public:
			void DoSetCount(unsigned int count) override;
			unsigned int DoGetCount() const override;
			void DoUpdate(unsigned int idx) override;
			void DoScrollHorz(int dx) override;
			void DoSetColumnsOrder(const wxArrayInt& order) override;
			wxArrayInt DoGetColumnsOrder() const override;
			WXDWORD MSWGetStyle(long style, WXDWORD* exstyle) const override;
			bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM* result) override;

			void Init();
			void DoInsertItem(const wxHeaderColumn& col, unsigned int idx);
			int GetShownColumnsCount() const
			{
				const int numItems = Header_GetItemCount(GetHandle());

				wxASSERT_MSG(numItems >= 0 && (unsigned)numItems <= m_numColumns,
							 "unexpected number of items in the native control");

				return numItems;
			}

			int MSWToNativeIdx(int idx)
			{
				// don't check for GetColumn(idx).IsShown() as it could have just became
				// false and we may be called from DoUpdate() to delete the old column
				wxASSERT_MSG(!m_isHidden[idx],
							 "column must be visible to have an "
							 "index in the native control");

				int item = idx;
				for (int i = 0; i < idx; i++)
				{
					if (GetColumn(i).IsHidden())
						item--; // one less column the native control knows about
				}

				wxASSERT_MSG(item >= 0 && item <= GetShownColumnsCount(), "logic error");

				return item;
			}
			int MSWFromNativeIdx(int item)
			{
				wxASSERT_MSG(item >= 0 && item < GetShownColumnsCount(),
							 "column index out of range");

				// reverse the above function

				unsigned idx = item;
				for (unsigned n = 0; n < m_numColumns; n++)
				{
					if (n > idx)
						break;

					if (GetColumn(n).IsHidden())
						idx++;
				}

				wxASSERT_MSG(MSWToNativeIdx(idx) == item, "logic error");

				return idx;
			}
			int MSWToNativeOrder(int pos)
			{
				wxASSERT_MSG(pos >= 0 && static_cast<unsigned>(pos) < m_numColumns,
							 "column position out of range");

				int order = pos;
				for (int n = 0; n < pos; n++)
				{
					if (GetColumn(m_colIndices[n]).IsHidden())
						order--;
				}

				wxASSERT_MSG(order >= 0 && order <= GetShownColumnsCount(), "logic error");

				return order;
			}
			int MSWFromNativeOrder(int order)
			{
				wxASSERT_MSG(order >= 0 && order < GetShownColumnsCount(),
							 "native column position out of range");

				unsigned pos = order;
				for (unsigned n = 0; n < m_numColumns; n++)
				{
					if (n > pos)
						break;

					if (GetColumn(m_colIndices[n]).IsHidden())
						pos++;
				}

				wxASSERT_MSG(MSWToNativeOrder(pos) == order, "logic error");

				return pos;
			}

			wxEventType GetClickEventType(bool dblclk, int button);
			wxMSWHeaderCtrlCustomDraw* GetCustomDraw();

		public:
			unsigned int m_numColumns;
			wxArrayInt m_isHidden;
			wxArrayInt m_colIndices;
			wxImageList* m_imageList;
			int m_scrollOffset;
			int m_colBeingDragged;
			bool m_isColBeingResized;
			wxMSWHeaderCtrlCustomDraw* m_customDraw;

		private:
			wxDECLARE_NO_COPY_CLASS(wxHeaderCtrlClone);
	};

	template<class T> wxHeaderCtrlClone& GetHeaderCtrlClone(T* headerCtrl)
	{
		wxHeaderCtrl* headerCtrlBase = const_cast<T*>(headerCtrl);
		return *reinterpret_cast<wxHeaderCtrlClone*>(headerCtrlBase);
	}
}

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
	
	wxRect HeaderCtrl::GetDropdownRect(size_t index) const
	{
		RECT rect = {};
		Header_GetItemDropDownRect(GetHandle(), index, &rect);
		return KxUtility::CopyRECTToRect(rect);
	}
	const wxHeaderColumn& HeaderCtrl::GetColumn(unsigned int index) const
	{
		return GetColumnAt(index)->GetNativeColumn();
	}
	bool HeaderCtrl::UpdateColumnWidthToFit(unsigned int index, int titleWidth)
	{
		Column* column = m_View->GetColumn(index);

		int contentsWidth = column->CalcBestSize();
		column->SetWidth(std::max({titleWidth, contentsWidth, column->GetMinWidth()}));
		m_View->OnColumnChange(index);
		return true;
	}
	void HeaderCtrl::UpdateColumn(size_t index)
	{
		wxHeaderCtrl::UpdateColumn(index);
	}
	
	void HeaderCtrl::DoUpdate(unsigned int index)
	{
		wxHeaderCtrlClone& clone = GetHeaderCtrlClone(this);

		// The native control does provide Header_SetItem() but it's inconvenient
		// to use it because it sends HDN_ITEMCHANGING messages and we'd have to
		// arrange not to block setting the width from there and the logic would be
		// more complicated as we'd have to reset the old values as well as setting
		// the new ones so instead just recreate the column

		const Column* column = GetColumnAt(index);
		if (!column->IsVisible())
		{
			// Column is hidden now, but it wasn't hidden before, so remove it
			if (!clone.m_isHidden[index])
			{
				Header_DeleteItem(GetHandle(), clone.MSWToNativeIdx(index));
				clone.m_isHidden[index] = true;
			}
			// Otherwise nothing to do, updating hidden column doesn't have any effect
		}
		else
		{
			// Column is shown now
			if (clone.m_isHidden[index])
			{
				clone.m_isHidden[index] = false;
			}
			else
			{
				// And it was shown before as well so we need to remove the old column
				Header_DeleteItem(GetHandle(), clone.MSWToNativeIdx(index));
			}
			DoInsertItem(*column, index);
		}
	}
	void HeaderCtrl::DoSetCount(unsigned int count)
	{
		wxHeaderCtrlClone& clone = GetHeaderCtrlClone(this);

		// First delete all old columns
		const size_t oldColumnsCount = clone.GetShownColumnsCount();
		for (size_t i = 0; i < oldColumnsCount; i++)
		{
			Header_DeleteItem(GetHandle(), 0);
		}

		// Update the column indices order array before changing m_numColumns
		DoResizeColumnIndices(clone.m_colIndices, count);

		// and add the new ones
		clone.m_numColumns = count;
		clone.m_isHidden.resize(clone.m_numColumns);
		for (size_t i = 0; i < count; i++)
		{
			const Column* column = GetColumnAt(i);
			if (column->IsVisible())
			{
				clone.m_isHidden[i] = false;
				DoInsertItem(*column, i);
			}
			else
			{
				// Hidden initially
				clone.m_isHidden[i] = true;
			}
		}
	}
	void HeaderCtrl::DoInsertItem(const Column& column, size_t index)
	{
		wxHeaderCtrlClone& clone = GetHeaderCtrlClone(this);
		wxASSERT_MSG(column.IsVisible(), "should only be called for shown columns");

		HDITEMW headerItem = {};

		// Title text
		headerItem.mask |= HDI_FORMAT|HDI_TEXT;
		headerItem.pszText = const_cast<wchar_t*>(column.m_Title.wc_str());
		headerItem.cchTextMax = column.m_Title.length();

		// Bitmap
		const wxBitmap& bitmap = column.m_Bitmap;
		if (bitmap.IsOk())
		{
			headerItem.mask |= HDI_IMAGE;
			if (HasFlag(wxHD_BITMAP_ON_RIGHT))
			{
				headerItem.fmt |= HDF_BITMAP_ON_RIGHT;
			}

			if (bitmap.IsOk())
			{
				if (!clone.m_imageList)
				{
					clone.m_imageList = new wxImageList(bitmap.GetWidth(), bitmap.GetHeight());
					Header_SetImageList(GetHandle(), clone.m_imageList->GetHIMAGELIST());
				}
				clone.m_imageList->Add(bitmap);
				headerItem.iImage = clone.m_imageList->GetImageCount() - 1;
			}
			else
			{
				// No bitmap but we still need to update the item
				headerItem.iImage = I_IMAGENONE;
			}
		}

		// Alignment
		if (column.GetTitleAlignment() != wxALIGN_NOT)
		{
			headerItem.mask |= HDF_LEFT;
			switch (column.GetTitleAlignment())
			{
				case wxALIGN_LEFT:
				{
					headerItem.fmt |= HDF_LEFT;
					break;
				}
				case wxALIGN_CENTER:
				case wxALIGN_CENTER_HORIZONTAL:
				{
					headerItem.fmt |= HDF_CENTER;
					break;
				}
				case wxALIGN_RIGHT:
				{
					headerItem.fmt |= HDF_RIGHT;
					break;
				}
			};
		}

		// Sort order
		if (column.IsSorted())
		{
			headerItem.fmt |= column.IsSortedAscending() ? HDF_SORTUP : HDF_SORTDOWN;
		}

		// Width
		if (column.GetWidthDescriptor() != wxCOL_WIDTH_DEFAULT)
		{
			headerItem.mask |= HDI_WIDTH;
			headerItem.cxy = column.GetWidth();
		}

		// Display order
		headerItem.mask |= HDI_ORDER;
		headerItem.iOrder = clone.MSWToNativeOrder(clone.m_colIndices.Index(index));

		// Dropdown
		// It seems that 'HDF_SPLITBUTTON' doesn't respect 'iOrder' value and it's always
		// displayed at actual item index. Though it can still be useful to allow this option.
		if (column.HasDropdown())
		{
			headerItem.fmt |= HDF_SPLITBUTTON;
		}

		// Checkbox
		if (column.HasCheckBox())
		{
			headerItem.fmt |= HDF_CHECKBOX;
			headerItem.fmt |= column.IsChecked() ? HDF_CHECKED : 0;
		}

		// Insert the item
		Header_InsertItem(GetHandle(), clone.MSWToNativeIdx(index), &headerItem);

		// Resizing cursor that correctly reflects per-column IsResizable() cannot
		// be implemented, it is per-control rather than per-column in the native
		// control. Enable resizing cursor if at least one column is resizeble.
		auto HasResizableColumns = [this]()
		{
			for (const auto& column: m_View->m_Columns)
			{
				if (column->IsVisible() && column->IsSizeable())
				{
					return true;
				}
			}
			return false;
		};
		KxUtility::ToggleWindowStyle(GetHandle(), GWL_STYLE, HDS_NOSIZING, !HasResizableColumns());
	}
	bool HeaderCtrl::MSWHandleNotify(WXLRESULT* result, int notification, WXWPARAM wParam, WXLPARAM lParam)
	{
		wxHeaderCtrlClone& clone = GetHeaderCtrlClone(this);
		const NMHEADERW* header = reinterpret_cast<NMHEADERW*>(lParam);

		switch (notification)
		{
			#if 0
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
			#endif

			case (int)HDN_ENDTRACKA:
			case (int)HDN_ENDTRACKW:
			{
				// Prevents the column shrinking to a size less than its minimum width
				const Column* column = GetColumnAt(clone.MSWFromNativeIdx(header->iItem));
				if (column && header->pitem->cxy <= column->GetMinWidth())
				{
					*result = TRUE;
				}
				return true;
			}
			case (int)HDN_ITEMSTATEICONCLICK:
			{
				Column* column = GetColumnAt(clone.MSWFromNativeIdx(header->iItem));
				if (column)
				{
					// Send an event, if it wasn't processed, toggle check state ourselves.
					// In any case update native column state after the event handler returns.
					const bool isChecked = column->IsChecked();
					if (!SendEvent(EVENT_COLUMN_TOGGLE, column->GetIndex()))
					{
						column->SetChecked(!isChecked);
					}
					UpdateColumn(column->GetIndex());
				}
				return true;
			}
			case (int)HDN_DROPDOWN:
			{
				SendEvent(EVENT_COLUMN_DROPDOWN, clone.MSWFromNativeIdx(header->iItem), GetDropdownRect(header->iItem));
				return true;
			}
		};
		return false;
	}

	HeaderCtrl::HeaderCtrl(View* parent)
		:wxHeaderCtrl(parent), m_View(parent)
	{
		// See comment in 'HeaderCtrl::SetBackgroundColour' for details
		// about why double-buffering needs to be disabled.
		SetDoubleBuffered(false);
		
		// Needed to display checkboxes
		KxUtility::ToggleWindowStyle(GetHandle(), GWL_STYLE, HDS_CHECKBOXES, true);

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
	
	bool HeaderCtrl::SetBackgroundColour(const wxColour& color)
	{
		// Skip setting the background color altogether to prevent 'wxHeaderCtrl' from trying to
		// owner-draw background because it causes flicker on resizing columns when double-buffering
		// is not enabled, but enabling double-buffering have some negative consequences on drag image.
		// Look for details in 'HeaderCtrl::MSWHandleNotify' in disabled code for 'HDN_[BEGIN|END]DRAG' messages.
		return false;
	}
}
