#include "KxStdAfx.h"
#include "HeaderCtrl.h"
#include "View.h"
#include "MainWindow.h"
#include "Column.h"
#include "KxFramework/KxDataView2Event.h"
#include "KxFramework/KxUtility.h"
#include <CommCtrl.h>

namespace
{
	enum class MouseButton
	{
		Left = 0,
		Right = 1,
		Middle = 2,
	};
	wxEventTypeTag<wxHeaderCtrlEvent> GetClickEventType(bool isDoubleClick, MouseButton button)
	{
		switch (button)
		{
			case MouseButton::Left:
			{
				return isDoubleClick ? wxEVT_HEADER_DCLICK : wxEVT_HEADER_CLICK;
			}
			case MouseButton::Right:
			{
				return isDoubleClick ? wxEVT_HEADER_RIGHT_DCLICK : wxEVT_HEADER_RIGHT_CLICK;
			}
			case MouseButton::Middle:
			{
				return isDoubleClick ? wxEVT_HEADER_MIDDLE_DCLICK : wxEVT_HEADER_MIDDLE_CLICK;
			}
		};
		return wxEVT_NULL;
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

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column)
		{
			if (SendEvent(EvtCOLUMN_HEADER_CLICK, column->GetIndex()))
			{
				return;
			}

			// Default handling for the column click is to sort by this column or  toggle its sort order
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
			m_View->OnColumnChange(*column);
			SendEvent(EvtCOLUMN_SORTED, column->GetIndex());
		}
	}
	void HeaderCtrl::OnRClick(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column)
		{
			// Event wasn't processed somewhere, use default behavior
			if (!SendEvent(EvtCOLUMN_HEADER_RCLICK, column->GetIndex()))
			{
				event.Skip();
				ToggleSortByColumn(column->GetIndex());
			}
		}
	}
	void HeaderCtrl::OnResize(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column)
		{
			column->AssignWidth(event.GetWidth());
			GetMainWindow()->UpdateDisplay();
		}
	}
	void HeaderCtrl::OnReordered(wxHeaderCtrlEvent& event)
	{
		FinishEditing();

		Column* column = m_View->GetColumn(event.GetColumn());
		if (column)
		{
			m_View->ColumnMoved(*column, event.GetNewOrder());
		}
	}
	void HeaderCtrl::OnWindowClick(wxMouseEvent& event)
	{
		wxPoint pos = event.GetPosition();
		if (pos.x > m_View->GetMainWindow()->GetRowWidth())
		{
			SendEvent(event.GetEventType() == wxEVT_LEFT_UP ? EvtCOLUMN_HEADER_CLICK : EvtCOLUMN_HEADER_RCLICK, -1);
		}
		event.Skip();
	}
	
	const wxHeaderColumn& HeaderCtrl::GetColumn(unsigned int index) const
	{
		return GetColumnAt(index)->GetNativeColumn();
	}
	bool HeaderCtrl::UpdateColumnWidthToFit(unsigned int index, int titleWidth)
	{
		if (Column* column = m_View->GetColumn(index))
		{
			int contentsWidth = column->CalcBestSize();
			column->SetWidth(std::max({titleWidth, contentsWidth, column->GetMinWidth()}));
			m_View->OnColumnChange(*column);
			return true;
		}
		return false;
	}
	
	void HeaderCtrl::DoUpdate(unsigned int)
	{
		DoSetCount();
	}
	void HeaderCtrl::DoSetCount(unsigned int)
	{
		// First delete all old columns
		const size_t oldColumnsCount = Header_GetItemCount(GetHandle());
		for (size_t i = 0; i < oldColumnsCount; i++)
		{
			Header_DeleteItem(GetHandle(), 0);
		}

		// Clear image list
		if (m_ImageList)
		{
			m_ImageList->Clear();
		}

		// And add the new ones
		for (const auto& column: m_View->m_Columns)
		{
			if (column->IsVisible())
			{
				DoInsertItem(*column);
			}
		}
	}
	void HeaderCtrl::DoInsertItem(const Column& column)
	{
		HDITEMW headerItem = {};
		headerItem.mask |= HDI_FORMAT|HDI_TEXT|HDI_LPARAM;
		headerItem.lParam = reinterpret_cast<LPARAM>(&column);

		// Title text
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
				if (!m_ImageList)
				{
					m_ImageList = std::make_unique<KxImageList>(bitmap.GetWidth(), bitmap.GetHeight());
					Header_SetImageList(GetHandle(), m_ImageList->GetHIMAGELIST());
				}
				headerItem.iImage = m_ImageList->Add(bitmap);
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
		headerItem.mask |= HDI_WIDTH;
		headerItem.cxy = column.GetWidth();

		// Display order
		headerItem.mask |= HDI_ORDER;
		headerItem.iOrder = column.GetPhysicalDisplayIndex();

		// Dropdown.
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
		Header_InsertItem(GetHandle(), headerItem.iOrder, &headerItem);

		// Resizing cursor that correctly reflects per-column IsSizable() cannot
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
	bool HeaderCtrl::MSWOnNotify(int ctrlID, WXLPARAM lParam, WXLPARAM* result)
	{
		const NMHEADERW* header = reinterpret_cast<NMHEADERW*>(lParam);
		const int notification = header->hdr.code;

		auto GetColumn = [this, header](bool hitTest = false)
		{
			if (hitTest)
			{
				return GetMainWindow()->HitTestColumn(m_View->ScreenToClient(::wxGetMousePosition()));
			}
			else
			{
				HDITEMW headerItem = {};
				headerItem.mask = HDI_LPARAM;
				Header_GetItem(GetHandle(), header->iItem, &headerItem);

				return reinterpret_cast<Column*>(headerItem.lParam);
			}
		};
		auto NewEvent = [this](wxEventType eventType, Column* column)
		{
			wxHeaderCtrlEvent event(eventType, GetId());
			event.SetEventObject(this);
			event.SetColumn(column ? column->GetIndex() : -1);

			return event;
		};
		auto Notify = [this, result](wxHeaderCtrlEvent& event, std::optional<bool> overrideResult = {})
		{
			// All of HDN_BEGIN{DRAG,TRACK}, HDN_TRACK and HDN_ITEMCHANGING
			// interpret TRUE in '*result' as a meaning to stop the control
			// default handling of the message.

			const bool disallowed = ProcessWindowEvent(event) && !event.IsAllowed();
			if (overrideResult)
			{
				*result = *overrideResult;
				return *overrideResult;
			}
			else if (disallowed)
			{
				*result = TRUE;
				return true;
			}
			return false;
		};

		switch (notification)
		{
			// Click
			case (int)HDN_ITEMCLICK:
			case (int)HDN_ITEMDBLCLICK:
			{
				m_DraggedColumn = nullptr;
				if (Column* column = GetColumn())
				{
					wxEventType eventType = ::GetClickEventType(notification == HDN_ITEMDBLCLICK, static_cast<MouseButton>(header->iButton));
					wxHeaderCtrlEvent event = NewEvent(eventType, column);
					return Notify(event);
				}
				return false;
			}
			case (int)HDN_DIVIDERDBLCLICK:
			{
				if (Column* column = GetColumn())
				{
					wxHeaderCtrlEvent event = NewEvent(wxEVT_HEADER_SEPARATOR_DCLICK, column);
					return Notify(event);
				}
				return false;
			}
			case NM_RCLICK:
			case NM_RDBLCLK:
			{
				// These two messages aren't from Header control and they contain no item index
				// and we have to use hit-testing to get the column.
				if (Column* column = GetColumn(true))
				{
					wxEventType eventType = ::GetClickEventType(notification == NM_RDBLCLK, MouseButton::Right);
					wxHeaderCtrlEvent event = NewEvent(eventType, column);
					return Notify(event);
				}
				return false;
			}

			// Resizing
			case (int)HDN_BEGINTRACKA:
			case (int)HDN_BEGINTRACKW:
			{
				if (!m_ResizedColumn)
				{
					// These messages are from header control but active area of grabbing column edge
					// is wider than column rect and hit-testing won't work here.
					Column* column = GetColumn();
					if (column && column->IsSizeable())
					{
						m_ResizedColumn = column;
						wxHeaderCtrlEvent event = NewEvent(wxEVT_HEADER_BEGIN_RESIZE, m_ResizedColumn);
						return Notify(event);
					}
				}
				return false;
			}
			case (int)HDN_ENDTRACKA:
			case (int)HDN_ENDTRACKW:
			{
				if (m_ResizedColumn)
				{
					bool preventShrink = false;

					int width = header->pitem->cxy;
					if (width < m_ResizedColumn->GetMinWidth())
					{
						width = m_ResizedColumn->GetMinWidth();
						preventShrink = true;
					}

					wxHeaderCtrlEvent event = NewEvent(wxEVT_HEADER_END_RESIZE, m_ResizedColumn);
					event.SetWidth(width);

					m_ResizedColumn = nullptr;
					return Notify(event, preventShrink);
				}
				return false;
			}
			case (int)HDN_TRACK:
			case (int)HDN_ITEMCHANGING:
			{
				if (m_ResizedColumn && header->pitem && (header->pitem->mask & HDI_WIDTH))
				{
					int width = header->pitem->cxy;
					bool preventShrink = false;

					if (width < m_ResizedColumn->GetMinWidth())
					{
						width = m_ResizedColumn->GetMinWidth();
						preventShrink = true;
					}

					wxHeaderCtrlEvent event = NewEvent(wxEVT_HEADER_RESIZING, m_ResizedColumn);
					event.SetWidth(width);
					return Notify(event, preventShrink);
				}
				return false;
			}

			// Reordering
			case (int)HDN_BEGINDRAG:
			{
				if (!m_DraggedColumn)
				{
					Column* column = GetColumn();
					if (column && column->IsMoveable())
					{
						m_DraggedColumn = column;

						wxHeaderCtrlEvent event = NewEvent(wxEVT_HEADER_BEGIN_REORDER, m_DraggedColumn);
						return Notify(event);
					}
				}
				return false;
			}
			case (int)HDN_ENDDRAG:
			{
				const int order = header->pitem->iOrder;
				if (m_DraggedColumn && order != -1)
				{
					Column* column = m_DraggedColumn;
					m_DraggedColumn = nullptr;
					m_UpdateColumns = true;

					wxHeaderCtrlEvent event = NewEvent(wxEVT_HEADER_END_REORDER, column);
					event.SetNewOrder(order);
					return Notify(event);
				}
				break;
			}
			case NM_RELEASEDCAPTURE:
			{
				if (m_DraggedColumn)
				{
					wxHeaderCtrlEvent event = NewEvent(wxEVT_HEADER_DRAGGING_CANCELLED, m_DraggedColumn);
					m_DraggedColumn = nullptr;
					return Notify(event);
				}
				return false;
			}
			
			// Checkbox
			case (int)HDN_ITEMSTATEICONCLICK:
			{
				if (Column* column = GetColumn())
				{
					// Send an event, if it wasn't processed, toggle check state ourselves.
					// In any case update native column state after the event handler returns.
					const bool isChecked = column->IsChecked();
					if (!SendEvent(EvtCOLUMN_TOGGLE, column->GetIndex()))
					{
						column->SetChecked(!isChecked);
					}
					UpdateColumn(*column);
					return true;
				}
				return false;
			}

			// Dropdown
			case (int)HDN_DROPDOWN:
			{
				if (Column* column = GetColumn())
				{
					SendEvent(EvtCOLUMN_DROPDOWN, column->GetIndex(), GetDropdownRect(header->iItem));
					return true;
				}
				return false;
			}
		};
		return false;
	}
	void HeaderCtrl::OnInternalIdle()
	{
		if (m_UpdateColumns)
		{
			UpdateColumnCount();
			m_UpdateColumns = false;
		}
		wxHeaderCtrl::OnInternalIdle();
	}

	void HeaderCtrl::UpdateColumn(const Column& column)
	{
		DoUpdate();
	}
	void HeaderCtrl::UpdateColumnCount()
	{
		DoSetCount();
	}

	wxRect HeaderCtrl::GetDropdownRect(size_t index) const
	{
		RECT rect = {};
		Header_GetItemDropDownRect(GetHandle(), index, &rect);
		return KxUtility::CopyRECTToRect(rect);
	}
	wxRect HeaderCtrl::GetDropdownRect(const Column& column) const
	{
		return GetDropdownRect(column.GetPhysicalDisplayIndex());
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

	MainWindow* HeaderCtrl::GetMainWindow() const
	{
		return m_View ? m_View->GetMainWindow() : nullptr;
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
			SendEvent(EvtCOLUMN_SORTED, column.GetIndex());
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
