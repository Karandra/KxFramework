#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/DataView/KxDataViewEditor.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewTreeNode.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/KxDataViewEvent.h"
#include "KxFramework/KxSystemSettings.h"
#include "KxFramework/KxSplashWindow.h"
#include "KxFramework/KxDCClipper.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxUxTheme.h"
#include "KxFramework/KxUxThemePartsAndStates.h"
#include "KxFramework/KxFrame.h"
#include <wx/popupwin.h>
#include <wx/generic/private/widthcalc.h>

class KxDataViewMainWindowMaxWidthCalculator: public wxMaxWidthCalculatorBase
{
	private:
		KxDataViewMainWindow* m_MainWindow = nullptr;
		KxDataViewRenderer* m_Renderer = nullptr;

		bool m_IsExpanderColumn = false;
		int m_ExpanderSize = 0;

	protected:
		void UpdateWithRow(int row) override
		{
			int indent = 0;
			KxDataViewItem item;

			if (m_IsExpanderColumn)
			{
				KxDataViewTreeNode* node = m_MainWindow->GetTreeNodeByRow(row);
				item = node->GetItem();
				indent = m_MainWindow->GetOwner()->GetIndent() * node->GetIndentLevel() + m_ExpanderSize;
			}
			else
			{
				item = m_MainWindow->GetItemByRow(row);
			}

			m_Renderer->PrepareItemToDraw(item, KxDATAVIEW_CELL_NONE);
			UpdateWithWidth(m_Renderer->GetCellSize().x + indent);
		}

	public:
		KxDataViewMainWindowMaxWidthCalculator(KxDataViewMainWindow* mainWindow, KxDataViewRenderer* renderer, int expanderSize)
			:wxMaxWidthCalculatorBase(mainWindow->GetOwner()->GetColumnPosition(renderer->GetColumn())),
			m_MainWindow(mainWindow), m_Renderer(renderer), m_ExpanderSize(expanderSize)
		{
			m_IsExpanderColumn = !m_MainWindow->IsList() && m_MainWindow->GetOwner()->GetExpanderColumnOrFirstOne() == renderer->GetColumn();
		}
};

//////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewMainWindow, wxWindow);

/* Events */
void KxDataViewMainWindow::OnChar(wxKeyEvent& event)
{
	// Propagate the char event upwards
	wxKeyEvent eventForParent(event);
	eventForParent.SetEventObject(m_Owner);
	if (m_Owner->ProcessWindowEvent(eventForParent))
	{
		return;
	}
	if (m_Owner->HandleAsNavigationKey(event))
	{
		return;
	}

	// No item then nothing to do
	if (!HasCurrentRow())
	{
		event.Skip();
		return;
	}

	switch (event.GetKeyCode())
	{
		case WXK_RETURN:
		{
			if (event.HasModifiers())
			{
				event.Skip();
				break;
			}
			else
			{
				// Enter activates the item, i.e. sends KxEVT_DATAVIEW_ITEM_ACTIVATED to
				// it. Only if that event is not handled do we activate column renderer (which
				// is normally done by Space) or even inline editing.

				KxDataViewEvent le(KxEVT_DATAVIEW_ITEM_ACTIVATED);
				CreateEventTemplate(le, GetItemByRow(m_CurrentRow));

				if (m_Owner->ProcessWindowEvent(le))
				{
					break;
				}
				// Else fall through to WXK_SPACE handling
			}
		}
		case WXK_SPACE:
		{
			if (event.HasModifiers())
			{
				event.Skip();
				break;
			}
			else
			{
				// Space toggles activatable items or -- if not activatable --
				// starts inline editing (this is normally done using F2 on
				// Windows, but Space is common everywhere else, so use it too
				// for greater cross-platform compatibility).

				const KxDataViewItem item = GetItemByRow(m_CurrentRow);

				// Activate the current activatable column. If not column is focused (typically
				// because the user has full row selected), try to find the first activatable
				// column (this would typically be a checkbox and we don't want to force the user
				// to set focus on the checkbox column).
				KxDataViewColumn *activatableCol = FindColumnForEditing(item, KxDATAVIEW_CELL_ACTIVATABLE);
				if (activatableCol)
				{
					const wxRect cellRect = GetOwner()->GetItemRect(item, activatableCol);

					KxDataViewRenderer* renderer = activatableCol->GetRenderer();
					renderer->PrepareItemToDraw(item, GetCellStateForRow(m_CurrentRow));
					renderer->OnActivateCell(item, cellRect, nullptr);

					break;
				}
				// Else fall through to WXK_F2 handling
			}
		}
		case WXK_F2:
		{
			if (event.HasModifiers())
			{
				event.Skip();
				break;
			}
			else
			{
				if (!m_Selection.IsEmpty())
				{
					// Mimic Windows 7 behavior: edit the item that has focus
					// if it is selected and the first selected item if focus
					// is out of selection.
					size_t selectedRow = INVALID_ROW;
					if (m_Selection.IsSelected(m_CurrentRow))
					{
						selectedRow = m_CurrentRow;
					}
					else
					{
						// Focused item is not selected.
						wxSelectionStore::IterationState cookie;
						selectedRow = m_Selection.GetFirstSelectedItem(cookie);
					}
					const KxDataViewItem item = GetItemByRow(selectedRow);

					// Edit the current column. If no column is focused
					// (typically because the user has full row selected), try
					// to find the first editable column.
					KxDataViewColumn *editableCol = FindColumnForEditing(item, KxDATAVIEW_CELL_EDITABLE);
					if (editableCol)
					{
						GetOwner()->EditItem(item, editableCol);
					}
				}
			}
			break;
		}
		case WXK_UP:
		{
			OnVerticalNavigation(event, -1);
			break;
		}
		case WXK_DOWN:
		{
			OnVerticalNavigation(event, +1);
			break;
			// Add the process for tree expanding/collapsing
		}
		case WXK_LEFT:
		{
			OnLeftKey(event);
			break;
		}
		case WXK_RIGHT:
		{
			OnRightKey(event);
			break;
		}
		case WXK_END:
		{
			OnVerticalNavigation(event, +(int)GetRowCount());
			break;
		}
		case WXK_HOME:
		{
			OnVerticalNavigation(event, -(int)GetRowCount());
			break;
		}
		case WXK_PAGEUP:
		{
			OnVerticalNavigation(event, -(int)(GetCountPerPage() - 1));
			break;
		}
		case WXK_PAGEDOWN:
		{
			OnVerticalNavigation(event, +(int)(GetCountPerPage() - 1));
			break;
		}
		default:
		{
			event.Skip();
		}
	};
}
void KxDataViewMainWindow::OnCharHook(wxKeyEvent& event)
{
	if (m_CurrentEditor)
	{
		// Handle any keys special for the in-place editor and return without calling Skip() below.
		switch (event.GetKeyCode())
		{
			case WXK_ESCAPE:
			{
				CancelEdit();
				return;
			}
			case WXK_RETURN:
			{
				// Shift-Enter is not special neither.
				if (event.ShiftDown())
				{
					break;
				}

				wxFALLTHROUGH;
			}
			case WXK_TAB:
			{
				// Ctrl/Alt-Tab or Enter could be used for something else, so don't handle them here.
				if (event.HasModifiers())
				{
					break;
				}

				EndEdit();
				return;
			}
		};
	}
	else if (m_UseCellFocus)
	{
		if (event.GetKeyCode() == WXK_TAB && !event.HasModifiers())
		{
			event.ShiftDown() ? OnLeftKey(event) : OnRightKey(event);
		}
	}
	event.Skip();
}
void KxDataViewMainWindow::OnVerticalNavigation(const wxKeyEvent& event, int delta)
{
	// If there is no selection, we cannot move it anywhere
	if (!HasCurrentRow() || IsEmpty())
	{
		return;
	}

	// Let's keep the new row inside the allowed range
	ptrdiff_t newRow = (ptrdiff_t)m_CurrentRow + delta;
	if (newRow < 0)
	{
		newRow = 0;
	}

	size_t rowCount = GetRowCount();
	if (newRow >= (ptrdiff_t)rowCount)
	{
		newRow = rowCount - 1;
	}

	size_t oldCurrent = m_CurrentRow;
	size_t newCurrent = newRow;
	if (newCurrent == oldCurrent)
	{
		return;
	}

	// In single selection we just ignore Shift as we can't select several items anyhow
	if (event.ShiftDown() && !IsSingleSelection())
	{
		RefreshRow(oldCurrent);
		ChangeCurrentRow(newCurrent);

		// Select all the items between the old and the new one
		if (oldCurrent > newCurrent)
		{
			newCurrent = oldCurrent;
			oldCurrent = m_CurrentRow;
		}
		SelectRows(oldCurrent, newCurrent);

		wxSelectionStore::IterationState cookie;
		size_t firstSel = m_Selection.GetFirstSelectedItem(cookie);
		if (firstSel != wxSelectionStore::NO_SELECTION)
		{
			SendSelectionChangedEvent(firstSel, m_CurrentColumn);
		}
	}
	else // !shift
	{
		RefreshRow(oldCurrent);

		// All previously selected items are unselected unless ctrl is held
		if (!event.ControlDown())
		{
			UnselectAllRows();
		}
		ChangeCurrentRow(newCurrent);

		if (!event.ControlDown())
		{
			SelectRow(m_CurrentRow, true);
			SendSelectionChangedEvent(m_CurrentRow, m_CurrentColumn);
		}
		else
		{
			RefreshRow(m_CurrentRow);
		}
	}
	GetOwner()->EnsureVisible(m_CurrentRow, INVALID_ROW);
}
void KxDataViewMainWindow::OnLeftKey(wxKeyEvent& event)
{
	if (IsList())
	{
		TryAdvanceCurrentColumn(nullptr, event, false);
	}
	else
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(m_CurrentRow);
		if (!node)
		{
			return;
		}
		if (TryAdvanceCurrentColumn(node, event, false))
		{
			return;
		}

		const bool dontCollapseNodes = event.GetKeyCode() == WXK_TAB;
		if (dontCollapseNodes)
		{
			m_CurrentColumn = nullptr;

			// allow focus change
			event.Skip();
			return;
		}

		// Because TryAdvanceCurrentColumn() return false, we are at the first
		// column or using whole-row selection. In this situation, we can use
		// the standard TreeView handling of the left key.
		if (node->HasChildren() && node->IsExpanded())
		{
			Collapse(m_CurrentRow);
		}
		else
		{
			// if the node is already closed, we move the selection to its parent
			KxDataViewTreeNode* parent_node = node->GetParent();
			if (parent_node)
			{
				size_t parent = GetRowByItem(parent_node->GetItem());
				if (parent != INVALID_ROW)
				{
					size_t row = m_CurrentRow;
					SelectRow(row, false);
					SelectRow(parent, true);
					ChangeCurrentRow(parent);
					GetOwner()->EnsureVisible(parent, INVALID_COLUMN);
					SendSelectionChangedEvent(parent_node->GetItem(), m_CurrentColumn);
				}
			}
		}
	}
}
void KxDataViewMainWindow::OnRightKey(wxKeyEvent& event)
{
	if (IsList())
	{
		TryAdvanceCurrentColumn(nullptr, event, true);
	}
	else
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(m_CurrentRow);
		if (!node)
		{
			return;
		}

		if (node->HasChildren())
		{
			if (!node->IsExpanded())
			{
				Expand(m_CurrentRow);
			}
			else
			{
				// If the node is already open, we move the selection to the first child
				size_t row = m_CurrentRow;
				SelectRow(row, false);
				SelectRow(row + 1, true);
				ChangeCurrentRow(row + 1);
				GetOwner()->EnsureVisible(row + 1, INVALID_COLUMN);
				SendSelectionChangedEvent(row + 1, m_CurrentColumn);
			}
		}
		else
		{
			TryAdvanceCurrentColumn(node, event, /*forward=*/true);
		}
	}
}

void KxDataViewMainWindow::OnMouse(wxMouseEvent& event)
{
	auto ResetHotTrackedExpander = [this]()
	{
		if (m_TreeNodeUnderMouse)
		{
			size_t row = GetRowByItem(m_TreeNodeUnderMouse->GetItem());
			m_TreeNodeUnderMouse = nullptr;
			RefreshRow(row);
		}
	};

	const wxEventType eventType = event.GetEventType();
	if (eventType == wxEVT_LEAVE_WINDOW || eventType == wxEVT_ENTER_WINDOW)
	{
		ResetHotTrackedExpander();

		event.Skip();
		return;
	}

	auto IsMouseInsideWindow = [this, &event]()
	{
		const int x = event.GetX();
		const int y = event.GetY();
		wxSize size = m_Owner->GetClientSize();

		return x >= 0 && y >= 0 && x <= size.GetWidth() && y <= size.GetHeight();
	};
	const bool isMouseInsideWindow = IsMouseInsideWindow();

	if (eventType == wxEVT_MOUSEWHEEL)
	{
		if (isMouseInsideWindow)
		{
			// Adjust Y mouse coordinate when vertical scrolling happened.
			// If we don't do this, row a few lines above or below will be tracked
			// and not the one, that should be tracked.
			if (event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
			{
				const int scrollPos = m_Owner->GetScrollPos(wxVERTICAL);
				if (scrollPos > 0 && scrollPos + m_Owner->GetScrollPageSize(wxVERTICAL) < m_Owner->GetScrollLines(wxVERTICAL))
				{
					int rateX = 0;
					int rateY = 0;
					m_Owner->GetScrollPixelsPerUnit(&rateX, &rateY);

					// Invert modifier if we scrolling up
					const bool isScrollDown = event.GetWheelRotation() < 0;
					if (!isScrollDown)
					{
						rateY = -rateY;
					}

					event.SetY(event.GetY() + event.GetLinesPerAction() * rateY);
				}
			}
		}
		event.Skip();
	}

	if (event.ButtonDown())
	{
		// Not skipping button down events would prevent the system from
		// setting focus to this window as most (all?) of them do by default,
		// so skip it to enable default handling.
		event.Skip();
	}

	int x = event.GetX();
	int y = event.GetY();
	m_Owner->CalcUnscrolledPosition(x, y, &x, &y);
	KxDataViewColumn* col = nullptr;

	int xpos = 0;
	size_t columnsCount = GetOwner()->GetColumnCount();
	for (size_t i = 0; i < columnsCount; i++)
	{
		KxDataViewColumn* column = GetOwner()->GetColumnAt(i);

		int width = 0;
		if (column->IsExposed(width))
		{
			if (x < xpos + width)
			{
				col = column;
				break;
			}
			xpos += width;
		}
	}

	KxDataViewModel* model = GetModel();

	const size_t current = GetLineAt(y);
	const KxDataViewItem item = GetItemByRow(current);

	// Hot track
	if (isMouseInsideWindow)
	{
		bool rowChnaged = m_HotTrackRow != current;
		bool columnChanged = m_HotTrackColumn != col;
		m_HotTrackColumn = col;

		if (rowChnaged || columnChanged)
		{
			if (rowChnaged)
			{
				m_HotTrackRowEnabled = false;
				RefreshRow(m_HotTrackRow);

				if (!item.IsOK())
				{
					ResetHotTrackedExpander();
				}
			}

			m_HotTrackRow = item.IsOK() ? current : INVALID_ROW;
			m_HotTrackRowEnabled = m_HotTrackRow != INVALID_ROW && m_HotTrackColumn;
			RefreshRow(m_HotTrackRow);

			KxDataViewEvent hoverEvent(KxEVT_DATAVIEW_ITEM_HOVERED);
			CreateEventTemplate(hoverEvent, item, m_HotTrackColumn);
			m_Owner->ProcessWindowEvent(hoverEvent);
		}
	}

	// Handle right clicking here, before everything else as context menu
	// events should be sent even when we click outside of any item, unlike all
	// the other ones.
	if (event.RightUp())
	{
		CancelEdit();

		KxDataViewEvent le(KxEVT_DATAVIEW_ITEM_CONTEXT_MENU);
		CreateEventTemplate(le, item, col);
		m_Owner->ProcessWindowEvent(le);
		return;
	}

	#if wxUSE_DRAG_AND_DROP
	if (event.Dragging() || ((m_DragCount > 0) && event.Leaving()))
	{
		if (m_DragCount == 0)
		{
			// We have to report the raw, physical coords as we want to be
			// able to call HitTest(event.m_pointDrag) from the user code to
			// get the item being dragged
			m_DragStart = event.GetPosition();
		}

		m_DragCount++;
		if ((m_DragCount < 3) && (event.Leaving()))
		{
			m_DragCount = 3;
		}
		else if (m_DragCount != 3)
		{
			return;
		}

		if (event.LeftIsDown())
		{
			m_Owner->CalcUnscrolledPosition(m_DragStart.x, m_DragStart.y, &m_DragStart.x, &m_DragStart.y);
			size_t drag_item_row = GetLineAt(m_DragStart.y);
			KxDataViewItem itemDragged = GetItemByRow(drag_item_row);

			// Don't allow invalid items
			if (itemDragged.IsOK())
			{
				// Notify cell about drag
				KxDataViewEventDND evt(KxEVT_DATAVIEW_ITEM_DRAG);
				CreateEventTemplate(evt, itemDragged, col);
				if (!m_Owner->HandleWindowEvent(evt) || !evt.IsAllowed())
				{
					return;
				}

				wxDataObject* obj = evt.GetDataObject();
				if (!obj)
				{
					return;
				}

				KxDataViewMainWindowDropSource drag(this, drag_item_row);
				drag.SetData(*obj);
				drag.DoDragDrop(evt.GetDragFlags());
				delete obj;
			}
		}
		return;
	}
	else
	{
		m_DragCount = 0;
	}
	#endif

	// Check if we clicked outside the item area.
	if ((current >= GetRowCount()) || !col)
	{
		// Follow Windows convention here: clicking either left or right (but
		// not middle) button clears the existing selection.
		if (m_Owner && (event.LeftDown() || event.RightDown()))
		{
			if (!m_Selection.IsEmpty())
			{
				m_Owner->UnselectAll();
				SendSelectionChangedEvent(KxDataViewItem(), col);
			}
		}
		event.Skip();
		return;
	}

	KxDataViewRenderer* cell = col->GetRenderer();
	KxDataViewColumn* expander = m_Owner->GetExpanderColumnOrFirstOne();

	// Test whether the mouse is hovering over the expander (a.k.a tree "+"
	// button) and also determine the offset of the real cell start, skipping
	// the indentation and the expander itself.
	bool hoverOverExpander = false;
	int itemOffset = 0;
	if (!IsList() && expander == col)
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(current);

		int indent = node->GetIndentLevel();
		itemOffset = GetOwner()->GetIndent() * indent;

		if (node->HasChildren())
		{
			// We make the rectangle we are looking in a bit bigger than the actual
			// visual expander so the user can hit that little thing reliably
			wxRect rect(xpos + itemOffset, GetLineStart(current) + (GetLineHeight(current) - m_UniformLineHeight) / 2, m_UniformLineHeight, m_UniformLineHeight);

			if (rect.Contains(x, y))
			{
				// So the mouse is over the expander
				hoverOverExpander = true;
				if (m_TreeNodeUnderMouse && m_TreeNodeUnderMouse != node)
				{
					RefreshRow(GetRowByItem(m_TreeNodeUnderMouse->GetItem()));
				}
				if (m_TreeNodeUnderMouse != node)
				{
					RefreshRow(current);
				}
				m_TreeNodeUnderMouse = node;
			}
		}

		// Account for the expander as well, even if this item doesn't have it,
		// its parent does so it still counts for the offset.
		itemOffset += m_UniformLineHeight;
	}
	if (!hoverOverExpander)
	{
		if (m_TreeNodeUnderMouse != nullptr)
		{
			// wxLogMessage("Undo the row: %d", GetRowByItem(m_underMouse->GetItem()));
			size_t row = GetRowByItem(m_TreeNodeUnderMouse->GetItem());
			m_TreeNodeUnderMouse = nullptr;
			RefreshRow(row);
		}
	}

	bool simulateClick = false;

	if (event.ButtonDClick())
	{
		m_EditorTimer.Stop();
		m_LastOnSame = false;
	}

	bool ignore_other_columns =	((expander != col) && (model->IsContainer(item)) && (!model->HasContainerColumns(item)));

	if (event.LeftDClick())
	{
		if (!hoverOverExpander && (current == m_LineLastClicked))
		{
			KxDataViewEvent le(KxEVT_DATAVIEW_ITEM_ACTIVATED);
			CreateEventTemplate(le, item, col);
			if (m_Owner->ProcessWindowEvent(le))
			{
				// Item activation was handled from the user code.
				return;
			}

			if (item.IsOK() && event.LeftDClick() && m_Owner->HasFlag(KxDV_DOUBLE_CLICK_EXPAND))
			{
				KxDataViewTreeNode* node = GetTreeNodeByRow(current);
				if (node && node->HasChildren())
				{
					ToggleExpand(current);
				}
			}
		}

		// Either it was a double click over the expander, or the second click
		// happened on another item than the first one or it was a bona fide
		// double click which was unhandled. In all these cases we continue
		// processing this event as a simple click, e.g. to select the item or
		// activate the renderer.
		simulateClick = true;
	}

	if (event.LeftUp() && !hoverOverExpander)
	{
		if (m_LineSelectSingleOnUp != INVALID_ROW)
		{
			// Select single line
			if (UnselectAllRows(m_LineSelectSingleOnUp))
			{
				SelectRow(m_LineSelectSingleOnUp, true);
				SendSelectionChangedEvent(m_LineSelectSingleOnUp, col);
			}
			// Else it was already selected, nothing to do.
		}

		// If the user click the expander, we do not do editing even if the column with expander are editable
		if (m_LastOnSame && !ignore_other_columns)
		{
			if ((col == m_CurrentColumn) && (current == m_CurrentRow) && IsCellEditable(item, col, KxDATAVIEW_CELL_EDITABLE))
			{
				m_EditorTimer.StartOnce(100);
			}
		}

		m_LastOnSame = false;
		m_LineSelectSingleOnUp = INVALID_ROW;
	}
	else if (!event.LeftUp())
	{
		// This is necessary, because after a DnD operation in
		// from and to ourself, the up event is swallowed by the
		// DnD code. So on next non-up event (which means here and
		// now) m_lineSelectSingleOnUp should be reset.
		m_LineSelectSingleOnUp = INVALID_ROW;
	}

	if (event.RightDown())
	{
		m_LineBeforeLastClicked = m_LineLastClicked;
		m_LineLastClicked = current;

		// If the item is already selected, do not update the selection.
		// Multi-selections should not be cleared if a selected item is clicked.
		if (!IsRowSelected(current))
		{
			UnselectAllRows();

			const size_t oldCurrent = m_CurrentRow;
			ChangeCurrentRow(current);
			SelectRow(m_CurrentRow, true);
			RefreshRow(oldCurrent);
			SendSelectionChangedEvent(m_CurrentRow, col);
		}
	}
	//else if (event.MiddleDown())
	//{
	//}

	
	if ((event.LeftDown() || simulateClick) && hoverOverExpander && !event.LeftDClick())
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(current);

		// hoverOverExpander being true tells us that our node must be
		// valid and have children.
		// So we don't need any extra checks.
		if (node->IsExpanded())
		{
			Collapse(current);
		}
		else
		{
			Expand(current);
		}
	}
	else if ((event.LeftDown() || simulateClick) && !hoverOverExpander)
	{
		m_LineBeforeLastClicked = m_LineLastClicked;
		m_LineLastClicked = current;

		size_t oldCurrentRow = m_CurrentRow;
		bool oldWasSelected = IsRowSelected(m_CurrentRow);

		bool cmdModifierDown = event.CmdDown();
		if (IsSingleSelection() || !(cmdModifierDown || event.ShiftDown()))
		{
			if (IsSingleSelection() || !IsRowSelected(current))
			{
				ChangeCurrentRow(current);
				if (UnselectAllRows(current))
				{
					SelectRow(m_CurrentRow, true);
					SendSelectionChangedEvent(m_CurrentRow, col);
				}
			}
			else 
			{
				// Multi selection & current is highlighted & no mod keys
				m_LineSelectSingleOnUp = current;
				ChangeCurrentRow(current); // change focus
			}
		}
		else
		{
			// Multi selection & either ctrl or shift is down
			if (cmdModifierDown)
			{
				ChangeCurrentRow(current);
				ReverseRowSelection(m_CurrentRow);
				SendSelectionChangedEvent(m_CurrentRow, col);
			}
			else if (event.ShiftDown())
			{
				ChangeCurrentRow(current);

				size_t lineFrom = oldCurrentRow;
				size_t lineTo = current;

				if (lineFrom == INVALID_ROW)
				{
					// If we hadn't had any current row before, treat this as a
					// simple click and select the new row only.
					lineFrom = current;
				}

				if (lineTo < lineFrom)
				{
					lineTo = lineFrom;
					lineFrom = m_CurrentRow;
				}

				SelectRows(lineFrom, lineTo);

				wxSelectionStore::IterationState cookie;
				size_t firstSel = m_Selection.GetFirstSelectedItem(cookie);
				if (firstSel != wxSelectionStore::NO_SELECTION)
				{
					SendSelectionChangedEvent(firstSel, col);
				}
			}
			else
			{
				// !ctrl, !shift
				// Test in the enclosing if should make it impossible
				wxFAIL_MSG(wxT("how did we get here?"));
			}
		}

		if (m_CurrentRow != oldCurrentRow)
		{
			RefreshRow(oldCurrentRow);
		}
		KxDataViewColumn* oldCurrentCol = m_CurrentColumn;

		// Update selection here...
		m_CurrentColumn = col;
		m_IsCurrentColumnSetByKeyboard = false;

		// This flag is used to decide whether we should start editing the item
		// label. We do it if the user clicks twice (but not double clicks,
		// i.e. simulateClick is false) on the same item but not if the click
		// was used for something else already, e.g. selecting the item (so it
		// must have been already selected) or giving the focus to the control
		// (so it must have had focus already).
		m_LastOnSame = !simulateClick && ((col == oldCurrentCol) &&	(current == oldCurrentRow)) && oldWasSelected && HasFocus();

		// Call ActivateCell() after everything else as under GTK+
		if (IsCellEditable(item, col, KxDATAVIEW_CELL_ACTIVATABLE))
		{
			// notify cell about click
			wxRect cell_rect(xpos + itemOffset, GetLineStart(current), col->GetWidth() - itemOffset, GetLineHeight(current));

			// Note that PrepareItemToDraw() should be called after GetLineStart()
			// call in cell_rect initialization above as GetLineStart() calls
			// PrepareItemToDraw() for other items from inside it.
			cell->PrepareItemToDraw(item, GetCellStateForRow(oldCurrentRow));

			// Report position relative to the cell's custom area, i.e.
			// not the entire space as given by the control but the one
			// used by the renderer after calculation of alignment etc.
			//
			// Notice that this results in negative coordinates when clicking
			// in the upper left corner of a center-aligned cell which doesn't
			// fill its column entirely so this is somewhat surprising, but we
			// do it like this for compatibility with the native GTK+ version,
			// see #12270.

			// adjust the rectangle ourselves to account for the alignment
			const int align = cell->GetEffectiveAlignment();

			wxRect rectItem = cell_rect;
			const wxSize size = cell->GetCellSize();
			if (size.x >= 0 && size.x < cell_rect.width)
			{
				if (align & wxALIGN_CENTER_HORIZONTAL)
				{
					rectItem.x += (cell_rect.width - size.x) / 2;
				}
				else if (align & wxALIGN_RIGHT)
				{
					rectItem.x += cell_rect.width - size.x;
				}
				// else: wxALIGN_LEFT is the default
			}

			if (size.y >= 0 && size.y < cell_rect.height)
			{
				if (align & wxALIGN_CENTER_VERTICAL)
				{
					rectItem.y += (cell_rect.height - size.y)/2;
				}
				else if (align & wxALIGN_BOTTOM)
				{
					rectItem.y += cell_rect.height - size.y;
				}
				// else: wxALIGN_TOP is the default
			}

			wxMouseEvent event2(event);
			event2.m_x -= rectItem.x;
			event2.m_y -= rectItem.y;
			m_Owner->CalcUnscrolledPosition(event2.m_x, event2.m_y, &event2.m_x, &event2.m_y);

			cell->OnActivateCell(item, cell_rect, &event2);
		}
	}

	// For selection rect
	if (event.LeftDown() || event.RightDown())
	{
		RefreshRow(m_CurrentRow);
	}
}
void KxDataViewMainWindow::OnSetFocus(wxFocusEvent& event)
{
	m_HasFocus = true;

	// Make the control usable from keyboard once it gets focus by ensuring
	// that it has a current row, if at all possible.
	if (!HasCurrentRow() && !IsEmpty())
	{
		ChangeCurrentRow(0);
	}
	if (HasCurrentRow())
	{
		Refresh();
	}
	event.Skip();
}
void KxDataViewMainWindow::OnKillFocus(wxFocusEvent& event)
{
	m_HasFocus = false;

	if (HasCurrentRow())
	{
		Refresh();
	}
	event.Skip();
}

bool KxDataViewMainWindow::SendExpanderEvent(wxEventType type, const KxDataViewItem& item)
{
	KxDataViewEvent event(type);
	CreateEventTemplate(event, item);

	return !m_Owner->ProcessWindowEvent(event) || event.IsAllowed();
}
void KxDataViewMainWindow::SendSelectionChangedEvent(const KxDataViewItem& item, KxDataViewColumn* column)
{
	RefreshRow(GetRowByItem(item));

	KxDataViewEvent event(KxEVT_DATAVIEW_ITEM_SELECTED);
	CreateEventTemplate(event, item, column);
	m_Owner->ProcessWindowEvent(event);
}
void KxDataViewMainWindow::SendSelectionChangedEvent(size_t row, KxDataViewColumn* column)
{
	SendSelectionChangedEvent(GetItemByRow(row), column);
}
bool KxDataViewMainWindow::SendEditingStartedEvent(const KxDataViewItem& item, KxDataViewEditor* editor)
{
	KxDataViewEvent event(KxEVT_DATAVIEW_ITEM_EDIT_STARTED);
	CreateEventTemplate(event, item, editor->GetColumn());

	m_Owner->ProcessWindowEvent(event);
	return event.IsAllowed();
}
bool KxDataViewMainWindow::SendEditingDoneEvent(const KxDataViewItem& item, KxDataViewEditor* editor, bool canceled, const wxAny& value)
{
	KxDataViewEvent event(KxEVT_DATAVIEW_ITEM_EDIT_DONE);
	CreateEventTemplate(event, item, editor->GetColumn());
	event.SetEditCanceled(canceled);
	event.GetValue() = value;

	m_Owner->ProcessWindowEvent(event);
	return event.IsAllowed();
}

/* Drawing */
void KxDataViewMainWindow::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);

	KxDataViewModel* model = GetModel();
	KxDataViewVirtualListModel* modelVirtualList = GetVirtualListModel();
	const wxSize clientSize = GetClientSize();

	dc.SetBrush(GetOwner()->GetBackgroundColour());
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(clientSize);

	if (IsEmpty())
	{
		// No items to draw.
		return;
	}

	// Prepare the DC
	GetOwner()->PrepareDC(dc);
	dc.SetFont(GetFont());

	wxRect update = GetUpdateRegion().GetBox();
	m_Owner->CalcUnscrolledPosition(update.x, update.y, &update.x, &update.y);

	// Compute which items needs to be redrawn
	size_t item_start = GetLineAt(std::max(0, update.y));
	size_t item_count = std::min(GetLineAt(std::max(0, update.y + update.height)) - item_start + 1, GetRowCount() - item_start);
	size_t item_last = item_start + item_count;

	// Send the event to wxDataViewCtrl itself.
	KxDataViewEvent cache_event(KxEVT_DATAVIEW_CACHE_HINT);
	CreateEventTemplate(cache_event);
	cache_event.SetCacheHints(item_start, item_last - 1);
	m_Owner->ProcessWindowEvent(cache_event);

	// Compute which columns needs to be redrawn
	size_t columnCount = GetOwner()->GetColumnCount();
	if (columnCount == 0)
	{
		// We assume that we have at least one column below and painting an empty control is unnecessary anyhow
		return;
	}

	int col_start = 0;
	int x_start = 0;
	for (col_start = 0; col_start < (int)columnCount; col_start++)
	{
		KxDataViewColumn* col = GetOwner()->GetColumnAt(col_start);

		int width = 0;
		if (col->IsExposed(width))
		{
			if (x_start + width >= update.x)
			{
				break;
			}
			x_start += width;
		}
	}

	int col_last = col_start;
	int x_last = x_start;
	for (; col_last < (int)columnCount; col_last++)
	{
		KxDataViewColumn* col = GetOwner()->GetColumnAt(col_last);

		int width = 0;
		if (col->IsExposed(width))
		{
			if (x_last > update.GetRight())
			{
				// If we drawing only part of the control, draw it one pixel wider,
				// to hide not drawn regions.
				if (x_last + width < GetEndOfLastCol())
				{
					x_last++;
				}
				break;
			}
			x_last += width;
		}
	}

	// Draw background of alternate rows specially if required
	if (m_Owner->HasFlag(KxDV_ALTERNATING_ROW_COLORS))
	{
		KxColor altRowColor = m_Owner->m_AlternateRowColor;
		if (!altRowColor)
		{
			// Determine the alternate rows color automatically from the background color.
			const wxColour bgColor = m_Owner->GetBackgroundColour();

			// Depending on the background, alternate row color will be 3% more dark or 50% brighter.
			int alpha = bgColor.GetRGB() > 0x808080 ? 97 : 150;
			altRowColor = bgColor.ChangeLightness(alpha);
		}

		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(altRowColor);

		// We only need to draw the visible part, so limit the rectangle to it.
		const int xRect = m_Owner->CalcUnscrolledPosition(wxPoint(0, 0)).x;
		const int widthRect = clientSize.x;
		for (size_t item = item_start; item < item_last; item++)
		{
			if (item % 2)
			{
				dc.DrawRectangle(xRect, GetLineStart(item), widthRect, GetLineHeight(item));
			}
		}
	}

	// Redraw the background for the items which are selected/current
	for (size_t item = item_start; item < item_last; item++)
	{
		bool selected = m_Selection.IsSelected(item);

		if (selected || item == m_CurrentRow)
		{
			wxRect rowRect(x_start, GetLineStart(item), x_last - x_start, GetLineHeight(item));
			bool renderColumnFocus = false;

			int flags = wxCONTROL_SELECTED;
			if (m_HasFocus)
			{
				flags |= wxCONTROL_FOCUSED;
			}

			// draw keyboard focus rect if applicable
			if (item == m_CurrentRow && m_HasFocus)
			{

				if (m_UseCellFocus && m_CurrentColumn && m_IsCurrentColumnSetByKeyboard)
				{
					renderColumnFocus = true;

					// If this is container node without columns, render full-row focus:
					if (!IsList())
					{
						KxDataViewTreeNode *node = GetTreeNodeByRow(item);
						if (node->HasChildren() && !model->HasContainerColumns(node->GetItem()))
						{
							renderColumnFocus = false;
						}
					}
				}

				if (renderColumnFocus)
				{
					wxRect colRect(rowRect);

					for (size_t i = col_start; i < (size_t)col_last; i++)
					{
						KxDataViewColumn* col = GetOwner()->GetColumnAt(i);

						int width = 0;
						if (col->IsExposed(width))
						{
							colRect.width = width;

							if (col == m_CurrentColumn)
							{
								// Draw selection rect left of column
								{
									wxRect clipRect(rowRect);
									clipRect.width = colRect.x;

									wxDCClipper clip(dc, clipRect);
									wxRendererNative::Get().DrawItemSelectionRect(this, dc, rowRect, flags);
								}

								// Draw selection rect right of column
								{
									wxRect clipRect(rowRect);
									clipRect.x = colRect.x + colRect.width;
									clipRect.width = rowRect.width - clipRect.x;

									wxDCClipper clip(dc, clipRect);
									wxRendererNative::Get().DrawItemSelectionRect(this, dc, rowRect, flags);
								}

								// Draw column selection rect
								wxRendererNative::Get().DrawItemSelectionRect(this, dc, colRect, flags|wxCONTROL_CURRENT|wxCONTROL_CELL);

								break;
							}

							colRect.x += colRect.width;
						}
					}
				}
			}

			// draw selection and whole-item focus:
			if (selected && !renderColumnFocus)
			{
				wxRendererNative::Get().DrawItemSelectionRect(this, dc, rowRect, flags|wxCONTROL_CURRENT);
			}
		}
	}

	if (m_HotTrackRowEnabled && m_HotTrackRow != INVALID_ROW && m_HotTrackColumn && !m_Owner->HasFlag(KxDV_NO_HOT_TRACK))
	{
		wxRect itemRect(x_start, GetLineStart(m_HotTrackRow), x_last - x_start, GetLineHeight(m_HotTrackRow));
		wxRendererNative::Get().DrawItemSelectionRect(this, dc, itemRect, wxCONTROL_FOCUSED|wxCONTROL_CURRENT);
	}

	#if wxUSE_DRAG_AND_DROP
	if (m_DropHint)
	{
		wxRect rect(x_start, GetLineStart(m_DropHintLine), x_last - x_start, GetLineHeight(m_DropHintLine));
		wxRendererNative::Get().DrawFocusRect(this, dc, rect, wxCONTROL_CURRENT);
	}
	#endif

	// Rules was here

	KxDataViewColumn* expander = m_Owner->GetExpanderColumnOrFirstOne();

	// Redraw all cells for all rows which must be repainted and all columns
	wxRect cell_rect;
	cell_rect.x = x_start;
	for (size_t i = col_start; i < (size_t)col_last; i++)
	{
		KxDataViewColumn* col = GetOwner()->GetColumnAt(i);
		KxDataViewRenderer* cell = col->GetRenderer();
		cell->SetDC(dc);

		cell_rect.width = col->GetWidth();

		if (col->IsHidden() || cell_rect.width <= 0)
		{
			continue;
		}

		wxGCDC gcDC(dc);
		cell->SetGCDC(gcDC);

		wxRect columnRect;
		columnRect.SetX(cell_rect.GetX());
		columnRect.SetY(0);
		columnRect.SetWidth(cell_rect.GetWidth());
		columnRect.SetHeight(m_virtualSize.GetHeight());

		wxDCClipper columnClip(dc, columnRect);
		wxDCClipper columnClipGC(gcDC, columnRect);

		for (size_t item = item_start; item < item_last; item++)
		{
			// Get the cell value and set it into the renderer
			KxDataViewTreeNode* node = nullptr;
			KxDataViewItem dataitem;

			if (modelVirtualList)
			{
				dataitem = modelVirtualList->GetItem(item);
			}
			else
			{
				node = GetTreeNodeByRow(item);
				if (!node)
				{
					continue;
				}
				dataitem = node->GetItem();

				// Skip all columns of "container" rows except the expander
				// column itself unless HasContainerColumns() overrides this.
				if (col != expander && model->IsContainer(dataitem) && !model->HasContainerColumns(dataitem))
				{
					continue;
				}
			}

			// Update cell_rect
			cell_rect.y = GetLineStart(item);
			cell_rect.height = GetLineHeight(item);

			KxDataViewCellState cellState = GetCellStateForRow(item);
			cell->PrepareItemToDraw(dataitem, cellState);

			// Draw the background
			if (i != col_start)
			{
				wxRect backgroundRect(cell_rect);
				backgroundRect.x--;
				backgroundRect.width++;
				DrawCellBackground(cell, backgroundRect, cellState);
			}
			else
			{
				DrawCellBackground(cell, cell_rect, cellState);
			}

			// deal with the expander
			int indent = 0;
			if ((!IsList()) && (col == expander))
			{
				// Calculate the indent first
				indent = GetOwner()->GetIndent() * node->GetIndentLevel();

				// we reserve m_lineHeight of horizontal space for the expander
				// but leave EXPANDER_MARGIN around the expander itself
				int exp_x = cell_rect.x + indent + EXPANDER_MARGIN;

				indent += m_UniformLineHeight;

				// draw expander if needed and visible
				if (node->HasChildren() && exp_x < cell_rect.GetRight())
				{
					dc.SetPen(m_PenExpander);
					dc.SetBrush(wxNullBrush);

					int exp_size = m_UniformLineHeight;// -2*EXPANDER_MARGIN; // Prevent expand button from shrinking
					int exp_y = cell_rect.y + (cell_rect.height - exp_size) / 2	+ EXPANDER_MARGIN - EXPANDER_OFFSET;

					const wxRect rect(exp_x, exp_y, exp_size, exp_size);

					int flag = 0;
					if (m_TreeNodeUnderMouse == node)
					{
						flag |= wxCONTROL_CURRENT;
					}
					if (node->IsExpanded())
					{
						flag |= wxCONTROL_EXPANDED;
					}

					// ensure that we don't overflow the cell (which might
					// happen if the column is very narrow)
					//wxDCClipper clip(dc, cell_rect);

					if (KxUxTheme theme(*this, KxUxThemeClass::TreeView); theme)
					{
						const int partID = flag & wxCONTROL_CURRENT ? TVP_HOTGLYPH : TVP_GLYPH;
						const int stateID = flag & wxCONTROL_EXPANDED ? GLPS_OPENED : GLPS_CLOSED;

						theme.DrawBackground(dc, partID, stateID, rect);
					}
					else
					{
						wxRendererNative::Get().DrawTreeItemButton(this, dc, rect, flag);
					}
				}

				// force the expander column to left-center align
				cell->SetAlignment(wxALIGN_CENTER_VERTICAL);
			}

			wxRect item_rect = cell_rect;
			item_rect.Deflate(PADDING_RIGHTLEFT, 0);

			// account for the tree indent (harmless if we're not indented)
			item_rect.x += indent;
			item_rect.width -= indent;

			if (item_rect.width <= 0)
			{
				continue;
			}

			// TODO:
			// It would be much more efficient to create a clipping
			// region for the entire column being rendered (in the OnPaint
			// of wxDataViewMainWindow) instead of a single clip region for
			// each cell. However it would mean that each renderer should
			// respect the given wxRect's top & bottom coords, eventually
			// violating only the left & right coords - however the user can
			// make its own renderer and thus we cannot be sure of that.

			wxDCClipper clip(dc, item_rect);
			////wxDCClipper clipGC(gcDC, item_rect);

			cell->CallDrawCellContent(item_rect, GetCellStateForRow(item));
		}

		cell_rect.x += cell_rect.width;
	}

	// Draw horizontal rules if required
	if (m_Owner->HasFlag(KxDV_HORIZ_RULES))
	{
		wxGCDC gcDC(dc);
		gcDC.SetPen(m_PenRuleH);
		gcDC.SetBrush(*wxTRANSPARENT_BRUSH);

		for (size_t i = item_start; i <= item_last; i++)
		{
			int y = GetLineStart(i);
			gcDC.DrawLine(x_start, y, x_last, y);
		}
	}

	// Draw vertical rules if required
	if (m_Owner->HasFlag(KxDV_VERT_RULES))
	{
		wxGCDC gcDC(dc);
		gcDC.SetPen(m_PenRuleV);
		gcDC.SetBrush(*wxTRANSPARENT_BRUSH);

		// Vertical rules are drawn in the last pixel of a column so that
		// they align perfectly with native MSW wxHeaderCtrl as well as for
		// consistency with MSW native list control. There's no vertical
		// rule at the most-left side of the control.

		int x = x_start - 1;
		for (size_t i = col_start; i < (size_t)col_last; i++)
		{
			KxDataViewColumn* col = GetOwner()->GetColumnAt(i);

			int width = 0;
			if (col->IsExposed(width))
			{
				x += width;
				gcDC.DrawLine(x, GetLineStart(item_start), x, GetLineStart(item_last) + clientSize.y);
			}
		}
	}
}
void KxDataViewMainWindow::DrawCellBackground(KxDataViewRenderer* renderer, const wxRect& rect, KxDataViewCellState cellState)
{
	renderer->CallDrawCellBackground(rect, cellState);
}
KxDataViewCellState KxDataViewMainWindow::GetCellStateForRow(size_t row) const
{
	KxDataViewCellState state = KxDATAVIEW_CELL_NONE;
	if (row != INVALID_ROW)
	{
		if (m_HasFocus && IsRowSelected(row))
		{
			state = state|KxDATAVIEW_CELL_SELECTED;
		}
		if (m_HotTrackRowEnabled && row == m_HotTrackRow && m_HotTrackColumn)
		{
			state = state|KxDATAVIEW_CELL_HIGHLIGHTED;
		}
		if (m_DropHint && row == m_DropHintLine)
		{
			state = state|KxDATAVIEW_CELL_DROP_TARGET;
		}
	}
	return state;
}

void KxDataViewMainWindow::UpdateDisplay()
{
	m_Dirty = true;
	m_TreeNodeUnderMouse = nullptr;
}
void KxDataViewMainWindow::RecalculateDisplay()
{
	if (GetModel())
	{
		SetVirtualSize(GetEndOfLastCol(), GetLineStart(GetRowCount()));
		GetOwner()->SetScrollRate(10, m_UniformLineHeight);
	}
	Refresh();
}

/* Columns */
void KxDataViewMainWindow::OnColumnsCountChanged()
{
	int editableCount = 0;

	size_t count = GetOwner()->GetColumnCount();
	for (size_t i = 0; i < count; i++)
	{
		KxDataViewColumn* column = GetOwner()->GetColumnAt(i);
		if (column->IsVisible() && column->IsEditable())
		{
			editableCount++;
		}
	}

	m_UseCellFocus = editableCount > 0;
	UpdateDisplay();
}
KxDataViewColumn* KxDataViewMainWindow::FindColumnForEditing(const KxDataViewItem& item, KxDataViewCellMode mode)
{
	// Edit the current column editable in 'mode'. If no column is focused
	// (typically because the user has full row selected), try to find the
	// first editable column (this would typically be a checkbox for
	// wxDATAVIEW_CELL_ACTIVATABLE and we don't want to force the user to set
	// focus on the checkbox column; or on the only editable text column).

	KxDataViewColumn* candidate = m_CurrentColumn;
	if (candidate && !IsCellEditable(item, candidate, mode) && !m_IsCurrentColumnSetByKeyboard)
	{
		// If current column was set by mouse to something not editable
		// and the user pressed Space/F2 to edit it, treat the
		// situation as if there was whole-row focus, because that's what is
		// visually indicated and the mouse click could very well be targeted
		// on the row rather than on an individual cell.
		//
		// But if it was done by keyboard, respect that even if the column
		// isn't editable, because focus is visually on that column and editing
		// something else would be surprising.
		candidate = nullptr;
	}

	if (!candidate)
	{
		size_t count = GetOwner()->GetColumnCount();
		for (size_t i = 0; i < count; i++)
		{
			KxDataViewColumn* column = GetOwner()->GetColumnAt(i);
			if (column->IsVisible())
			{
				if (IsCellEditable(item, column, KxDATAVIEW_CELL_EDITABLE))
				{
					candidate = column;
					break;
				}
			}
		}
	}

	// If on container item without columns, only the expander column may be directly editable:
	KxDataViewModel* model = GetModel();
	if (candidate && GetOwner()->GetExpanderColumn() != candidate && model->IsContainer(item) && !model->HasContainerColumns(item))
	{
		candidate = GetOwner()->GetExpanderColumn();
	}

	if (candidate && IsCellEditable(item, candidate, mode))
	{
		return candidate;
	}
	return nullptr;
}
size_t KxDataViewMainWindow::GetBestColumnWidth(size_t index) const
{
	auto& columnInfo = m_Owner->m_Columns[index];
	if (columnInfo->HasBestWidth())
	{
		return columnInfo->GetBestWidth();
	}

	const size_t count = GetRowCount();
	KxDataViewColumn* column = m_Owner->GetColumn(index);
	KxDataViewRenderer* renderer = column->GetRenderer();

	KxDataViewMainWindowMaxWidthCalculator calculator(const_cast<KxDataViewMainWindow*>(this), renderer, m_UniformLineHeight);
	calculator.UpdateWithWidth(column->GetMinWidth());

	if (m_Owner->HasHeaderCtrl())
	{
		calculator.UpdateWithWidth(m_Owner->GetHeaderCtrl()->GetColumnTitleWidth(*column->GetAsSettableHeaderColumn()));
	}

	const wxPoint origin = m_Owner->CalcUnscrolledPosition(wxPoint(0, 0));
	calculator.ComputeBestColumnWidth(count, GetLineAt(origin.y), GetLineAt(origin.y + GetClientSize().y));

	int maxWidth = calculator.GetMaxWidth();
	if (maxWidth > 0)
	{
		maxWidth += 2 * PADDING_RIGHTLEFT;
	}
	columnInfo->SetBestWidth(maxWidth);
	return maxWidth;
}

/* Items */
size_t KxDataViewMainWindow::RecalculateItemCount() const
{
	if (const KxDataViewVirtualListModel* modelVirtualList = GetVirtualListModel())
	{
		return modelVirtualList->GetItemCount();
	}
	else
	{
		return m_TreeRoot->GetSubTreeCount();
	}
}
bool KxDataViewMainWindow::DoItemChanged(const KxDataViewItem& item, KxDataViewColumn* column)
{
	// Move this node to its new correct place after it was updated.
	//
	// In principle, we could skip the call to PutInSortOrder() if the modified
	// column is not the sort column, but in real-world applications it's fully
	// possible and likely that custom compare uses not only the selected model
	// column but also falls back to other values for comparison. To ensure consistency
	// it is better to treat a value change as if it was an item change.
	KxDataViewTreeNode* node = FindNode(item);
	wxCHECK_MSG(node, false, "invalid item");
	node->PutInSortOrder(this);

	if (column == nullptr)
	{
		m_Owner->InvalidateColBestWidths();
	}
	else
	{
		m_Owner->InvalidateColBestWidth(m_Owner->GetColumnIndex(column));
	}

	// Update the displayed value(s).
	RefreshRow(GetRowByItem(item));

	// Send event
	KxDataViewEvent event(KxEVT_DATAVIEW_ITEM_VALUE_CHANGED);
	CreateEventTemplate(event, item, column, true);
	m_Owner->ProcessWindowEvent(event);

	return true;
}

/* Tree nodes */
KxDataViewTreeNode* KxDataViewMainWindow::FindNode(const KxDataViewItem& item)
{
	const KxDataViewModel* model = GetModel();
	if (model)
	{
		if (!item.IsOK())
		{
			return m_TreeRoot;
		}

		// Compose the parent-chain for the item we are looking for.
		KxDataViewItem::Vector parentChain;
		KxDataViewItem it(item);
		while (it.IsOK())
		{
			parentChain.push_back(it);
			it = model->GetParent(it);
		}

		// Find the item along the parent-chain.
		// This algorithm is designed to speed up the node-finding method
		KxDataViewTreeNode* node = m_TreeRoot;
		for (auto it = parentChain.rbegin(); it != parentChain.rend(); ++it)
		{
			const KxDataViewItem& parentChainItem = *it;
			if (node->HasChildren())
			{
				if (node->GetChildNodes().empty())
				{
					// Even though the item is a container, it doesn't have any child nodes
					// in the control's representation yet. We have to realize its subtree now.
					BuildTreeHelper(node->GetItem(), node);
				}

				bool nodeFound = false;
				for (KxDataViewTreeNode* currentNode: node->GetChildNodes())
				{
					if (currentNode->GetItem() == parentChainItem)
					{
						if (currentNode->GetItem() == item)
						{
							return currentNode;
						}

						node = currentNode;
						nodeFound = true;
						break;
					}
				}

				if (!nodeFound)
				{
					return nullptr;
				}
			}
		}
	}
	return nullptr;
}
KxDataViewTreeNode* KxDataViewMainWindow::GetTreeNodeByRow(size_t row) const
{
	wxASSERT(!IsVirtualList());

	if (row != INVALID_ROW)
	{
		KxDataViewTreeNodeOperation_RowToTreeNode operation(static_cast<ptrdiff_t>(row), -2);
		operation.Walk(m_TreeRoot);
		return operation.GetResult();
	}
	return nullptr;
}

void KxDataViewMainWindow::BuildTreeHelper(const KxDataViewItem& item, KxDataViewTreeNode* node)
{
	KxDataViewModel* model = GetModel();
	if (model->IsContainer(item))
	{
		KxDataViewItem::Vector children;
		model->GetChildren(item, children);

		size_t index = 0;
		for (const KxDataViewItem& childItem: children)
		{
			KxDataViewTreeNode* childNode = new KxDataViewTreeNode(node, childItem);
			if (model->IsContainer(childItem))
			{
				childNode->SetHasChildren(true);
			}

			node->InsertChild(this, childNode, index);
			index++;
		}

		wxASSERT(node->IsExpanded());
		node->ChangeSubTreeCount(children.size());
	}
}
void KxDataViewMainWindow::BuildTree(KxDataViewModel* model)
{
	DestroyTree();

	if (!GetModel()->IsVirtualListModel())
	{
		m_TreeRoot = KxDataViewTreeNode::CreateRootNode(this);

		// Use an invalid item to fetch the top-level elements
		BuildTreeHelper(KxDataViewItem(), m_TreeRoot);
	}
	InvalidateItemCount();
}
void KxDataViewMainWindow::DestroyTree()
{
	if (!IsVirtualList())
	{
		delete m_TreeRoot;
		m_TreeRoot = nullptr;

		m_ItemsCount = 0;
	}
}

/* Misc */
void KxDataViewMainWindow::OnInternalIdle()
{
	wxWindow::OnInternalIdle();

	if (!IsMouseInWindow() && m_HotTrackRow != -1)
	{
		m_HotTrackRowEnabled = false;
		RefreshRow(m_HotTrackRow);

		m_HotTrackRow = -1;
		m_HotTrackColumn = nullptr;
	}

	if (m_Dirty)
	{
		RecalculateDisplay();
		UpdateColumnSizes();
		m_Dirty = false;
	}
}
void KxDataViewMainWindow::OnEditorTimer(wxTimerEvent& event)
{
	if (!m_Owner->HasFlag(KxDV_NO_TIMEOUT_EDIT))
	{
		// We have to call this here because changes may just have
		// been made and no screen update taken place.
		if (m_Dirty)
		{
			// TODO:
			// Use wxTheApp->SafeYieldFor(nullptr, wxEVT_CATEGORY_UI) instead (needs to be tested!)
			wxSafeYield();
		}

		BeginEdit(GetItemByRow(m_CurrentRow), m_CurrentColumn);
	}
}

KxDataViewMainWindow::KxDataViewMainWindow(KxDataViewCtrl* parent,
										   wxWindowID id,
										   const wxPoint& pos,
										   const wxSize& size
)
	:wxWindow(parent, id, pos, size, wxWANTS_CHARS|wxBORDER_NONE, GetClassInfo()->GetClassName()),
	m_Owner(parent)
{
	// Setup drawing
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));

	KxColor rulesColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
	rulesColor.SetAlpha8(127);

	m_PenRuleH = rulesColor;
	m_PenRuleV = rulesColor;
	m_PenExpander = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	m_UniformLineHeight = GetDefaultRowHeight();

	// Create root node
	m_TreeRoot = KxDataViewTreeNode::CreateRootNode(this);

	// Bind events
	m_EditorTimer.BindFunction(&KxDataViewMainWindow::OnEditorTimer, this);

	Bind(wxEVT_PAINT, &KxDataViewMainWindow::OnPaint, this);
	Bind(wxEVT_SET_FOCUS, &KxDataViewMainWindow::OnSetFocus, this);
	Bind(wxEVT_KILL_FOCUS, &KxDataViewMainWindow::OnKillFocus, this);
	Bind(wxEVT_CHAR_HOOK, &KxDataViewMainWindow::OnCharHook, this);
	Bind(wxEVT_CHAR, &KxDataViewMainWindow::OnChar, this);

	Bind(wxEVT_LEFT_DOWN, &KxDataViewMainWindow::OnMouse, this);
	Bind(wxEVT_LEFT_UP, &KxDataViewMainWindow::OnMouse, this);
	Bind(wxEVT_LEFT_DCLICK, &KxDataViewMainWindow::OnMouse, this);

	//Bind(wxEVT_MIDDLE_DOWN, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_MIDDLE_UP, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_MIDDLE_DCLICK, &KxDataViewMainWindow::OnMouse, this);

	Bind(wxEVT_RIGHT_DOWN, &KxDataViewMainWindow::OnMouse, this);
	Bind(wxEVT_RIGHT_UP, &KxDataViewMainWindow::OnMouse, this);
	Bind(wxEVT_RIGHT_DCLICK, &KxDataViewMainWindow::OnMouse, this);

	Bind(wxEVT_MOTION, &KxDataViewMainWindow::OnMouse, this);
	Bind(wxEVT_ENTER_WINDOW, &KxDataViewMainWindow::OnMouse, this);
	Bind(wxEVT_LEAVE_WINDOW, &KxDataViewMainWindow::OnMouse, this);
	Bind(wxEVT_MOUSEWHEEL, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_CHILD_FOCUS, &KxDataViewMainWindow::OnMouse, this);
	
	//Bind(wxEVT_AUX1_DOWN, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_AUX1_UP, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_AUX1_DCLICK, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_AUX2_DOWN, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_AUX2_UP, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_AUX2_DCLICK, &KxDataViewMainWindow::OnMouse, this);
	//Bind(wxEVT_MAGNIFY, &KxDataViewMainWindow::OnMouse, this);

	// Do update
	UpdateDisplay();
}
KxDataViewMainWindow::~KxDataViewMainWindow()
{
	DestroyTree();
}

void KxDataViewMainWindow::CreateEventTemplate(KxDataViewEvent& event, const KxDataViewItem& item, KxDataViewColumn* column, bool noCurrent)
{
	event.SetId(m_Owner->GetId());
	event.SetEventObject(m_Owner);
	event.SetModel(GetModel());
	event.SetItem(item);
	event.SetColumn(column);
}

/* Model */
KxDataViewVirtualListModel* KxDataViewMainWindow::GetVirtualListModel()
{
	KxDataViewModel* model = GetModel();
	return model && model->IsVirtualListModel() ? static_cast<KxDataViewVirtualListModel*>(model) : nullptr;
}
const KxDataViewVirtualListModel* KxDataViewMainWindow::GetVirtualListModel() const
{
	const KxDataViewModel* model = GetModel();
	return model && model->IsVirtualListModel() ? static_cast<const KxDataViewVirtualListModel*>(model) : nullptr;
}

bool KxDataViewMainWindow::SwapTreeNodes(const KxDataViewItem& item1, const KxDataViewItem& item2)
{
	if (!IsVirtualList() && !IsList())
	{
		KxDataViewTreeNode* node1 = FindNode(item1);
		KxDataViewTreeNode* node2 = FindNode(item2);

		if (node1 && node2)
		{
			if (KxDataViewTreeNode::SwapNodes(node1, node2))
			{
				ItemChanged(item1);
				ItemChanged(item2);
				return true;
			}
		}
	}
	return false;
}

/* Notifications */
bool KxDataViewMainWindow::ItemAdded(const KxDataViewItem& parent, const KxDataViewItem& item)
{
	if (KxDataViewVirtualListModel* modelVirtualList = GetVirtualListModel())
	{
		m_ItemsCount = modelVirtualList->GetItemCount();
		m_Selection.OnItemsInserted(GetRowByItem(item), 1);
	}
	else
	{
		KxDataViewTreeNode* parentNode = FindNode(parent);
		if (parentNode)
		{
			KxDataViewModel* model = GetModel();

			KxDataViewTreeNode* itemNode = new KxDataViewTreeNode(parentNode, item);
			itemNode->SetHasChildren(GetModel()->IsContainer(item));
			parentNode->SetHasChildren(true);

			if (GetSortOrder().IsNone())
			{
				// There's no sorting, so we need to select an insertion position.
				KxDataViewItem::Vector modelSiblings;
				GetModel()->GetChildren(parent, modelSiblings);
				const size_t modelSiblingsSize = modelSiblings.size();

				// Search from end
				auto itemIt = std::find(modelSiblings.rbegin(), modelSiblings.rend(), item);
				size_t modelItemPosition = std::distance(modelSiblings.begin(), itemIt.base()) - 1;
				wxCHECK_MSG(itemIt != modelSiblings.rend(), false, "adding non-existent item?");

				const KxDataViewTreeNode::Vector& nodeSiblings = parentNode->GetChildNodes();
				const size_t nodeSiblingsSize = nodeSiblings.size();

				size_t nodePos = 0;
				if (modelItemPosition == modelSiblingsSize - 1)
				{
					nodePos = nodeSiblingsSize;
				}
				else if (modelSiblingsSize == nodeSiblingsSize + 1)
				{
					// This is the simple case when our node tree already matches the model and only this one item is missing.
					nodePos = modelItemPosition;
				}
				else
				{
					// It's possible that a larger discrepancy between the model and our realization exists.
					// This can happen e.g. when adding a bunch of items to the model and then calling ItemsAdded()
					// just once afterwards. In this case, we must find the right position by looking at sibling items.

					// Append to the end if we won't find a better position:
					nodePos = nodeSiblingsSize;

					for (size_t nextItemPos = modelItemPosition + 1; nextItemPos < modelItemPosition; nextItemPos++)
					{
						ptrdiff_t nextNodePos = parentNode->FindChildByItem(modelSiblings[nextItemPos]);
						if (nextNodePos != wxNOT_FOUND)
						{
							nodePos = nextNodePos;
							break;
						}
					}
				}
				parentNode->ChangeSubTreeCount(+1);
				parentNode->InsertChild(this, itemNode, nodePos);
			}
			else
			{
				// Node list is or will be sorted, so InsertChild do not need insertion position
				parentNode->ChangeSubTreeCount(+1);
				parentNode->InsertChild(this, itemNode, 0);
			}
			InvalidateItemCount();
		}
	}

	m_Owner->InvalidateColBestWidths();
	UpdateDisplay();
	return true;
}
bool KxDataViewMainWindow::ItemDeleted(const KxDataViewItem& parent, const KxDataViewItem& item)
{
	if (KxDataViewVirtualListModel* modelVirtualList = GetVirtualListModel())
	{
		m_ItemsCount = modelVirtualList->GetItemCount();
		m_Selection.OnItemDelete(GetRowByItem(item));
	}
	else
	{
		KxDataViewTreeNode* parentNode = FindNode(parent);

		// Notice that it is possible that the item being deleted is not in the
		// tree at all, for example we could be deleting a never shown (because
		// collapsed) item in a tree model. So it's not an error if we don't know
		// about this item, just return without doing anything then.
		if (!parentNode)
		{
			return true;
		}

		wxCHECK_MSG(parentNode->HasChildren(), false, "parent node doesn't have children?");
		const KxDataViewTreeNode::Vector& parentsChildren = parentNode->GetChildNodes();

		// We can't use FindNode() to find 'item', because it was already
		// removed from the model by the time ItemDeleted() is called, so we
		// have to do it manually. We keep track of its position as well for
		// later use.
		size_t itemNodePosition = 0;
		KxDataViewTreeNode* itemNode = nullptr;
		for (KxDataViewTreeNode* node: parentNode->GetChildNodes())
		{
			itemNodePosition++;
			if (node->GetItem() == item)
			{
				itemNode = node;
				break;
			}
		}

		// If the parent wasn't expanded, it's possible that we didn't have a
		// node corresponding to 'item' and so there's nothing left to do.
		if (!itemNode)
		{
			// If this was the last child to be removed, it's possible the parent
			// node became a leaf. Let's ask the model about it.
			if (parentNode->GetChildNodes().empty())
			{
				parentNode->SetHasChildren(GetModel()->IsContainer(parent));
			}
			return true;
		}

		// Delete the item from wxDataViewTreeNode representation:
		const int itemsDeleted = 1 + itemNode->GetSubTreeCount();

		parentNode->RemoveChild(itemNodePosition);
		parentNode->ChangeSubTreeCount(-itemsDeleted);

		// Make the row number invalid and get a new valid one when user call GetRowCount
		InvalidateItemCount();

		// If this was the last child to be removed, it's possible the parent
		// node became a leaf. Let's ask the model about it.
		if (parentNode->GetChildNodes().empty())
		{
			bool isContainer = GetModel()->IsContainer(parent);
			parentNode->SetHasChildren(isContainer);
			if (isContainer)
			{
				// If it's still a container, make sure we show "+" icon for it
				// and not "-" one as there is nothing to collapse any more.
				if (parentNode->IsExpanded())
				{
					parentNode->ToggleExpanded(this);
				}
			}
		}

		// Update selection by removing 'item' and its entire children tree from the selection.
		if (!m_Selection.IsEmpty())
		{
			// We can't call GetRowByItem() on 'item', as it's already deleted, so compute it from
			// the parent ('parentNode') and position in its list of children
			size_t itemRow = INVALID_ROW;
			if (itemNodePosition == 0)
			{
				// First child, row number is that of the parent parentNode + 1
				itemRow = GetRowByItem(parentNode->GetItem()) + 1;
			}
			else
			{
				// Row number is that of the sibling above 'item' + its subtree if any + 1
				if (parentNode->HasChildren())
				{
					const KxDataViewTreeNode* siblingNode = parentNode->GetChildNodes()[itemNodePosition - 1];
					itemRow = GetRowByItem(siblingNode->GetItem()) + siblingNode->GetSubTreeCount() + 1;
				}
			}
			m_Selection.OnItemsDeleted(itemRow, itemsDeleted);
		}
	}

	// Change the current row to the last row if the current exceed the max row number
	if (m_CurrentRow >= GetRowCount())
	{
		ChangeCurrentRow(m_ItemsCount - 1);
	}

	GetOwner()->InvalidateColBestWidths();
	UpdateDisplay();
	return true;
}
bool KxDataViewMainWindow::ItemChanged(const KxDataViewItem& item)
{
	return DoItemChanged(item, nullptr);
}
bool KxDataViewMainWindow::ValueChanged(const KxDataViewItem& item, KxDataViewColumn* column)
{
	return column ? DoItemChanged(item, column) : false;
}
bool KxDataViewMainWindow::ItemsCleared()
{
	DestroyTree();
	m_Selection.Clear();
	m_CurrentRow = INVALID_ROW;
	m_HotTrackRow = INVALID_ROW;
	m_CurrentColumn = nullptr;
	m_HotTrackColumn = nullptr;
	m_TreeNodeUnderMouse = nullptr;

	if (GetModel())
	{
		BuildTree(GetModel());
	}
	else
	{
		m_ItemsCount = 0;
	}

	GetOwner()->InvalidateColBestWidths();
	UpdateDisplay();
	return true;
}
void KxDataViewMainWindow::Resort()
{
	if (!IsVirtualList())
	{
		m_TreeRoot->Resort(this);
	}
	UpdateDisplay();
}

/* Rows refreshing */
void KxDataViewMainWindow::RefreshRows(size_t from, size_t to)
{
	wxRect rect = GetLinesRect(from, to);
	m_Owner->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);

	wxSize clientSize = GetClientSize();
	wxRect clientRect(0, 0, clientSize.x, clientSize.y);
	wxRect intersectRect = clientRect.Intersect(rect);
	if (!intersectRect.IsEmpty())
	{
		RefreshRect(intersectRect, true);
	}
}
void KxDataViewMainWindow::RefreshRowsAfter(size_t firstRow)
{
	wxSize clientSize = GetClientSize();
	int start = GetLineStart(firstRow);
	m_Owner->CalcScrolledPosition(start, 0, &start, nullptr);

	if (start <= clientSize.y)
	{
		wxRect rect(0, start, clientSize.x, clientSize.y - start);
		RefreshRect(rect, true);
	}
}

/* Item rect */
wxRect KxDataViewMainWindow::GetLinesRect(size_t rowFrom, size_t rowTo) const
{
	if (rowFrom > rowTo)
	{
		std::swap(rowFrom, rowTo);
	}

	wxRect rect;
	rect.x = 0;
	rect.y = GetLineStart(rowFrom);

	// Don't calculate exact width of the row, because GetEndOfLastCol() is
	// expensive to call, and controls with rows not spanning entire width rare.
	// It is more efficient to e.g. repaint empty parts of the window needlessly.
	rect.width = std::numeric_limits<decltype(rect.width)>::max();
	if (rowFrom == rowTo)
	{
		rect.height = GetLineHeight(rowFrom);
	}
	else
	{
		rect.height = GetLineStart(rowTo) - rect.y + GetLineHeight(rowTo);
	}
	return rect;
}
int KxDataViewMainWindow::GetLineStart(size_t row) const
{
	const KxDataViewModel* model = GetModel();
	const bool modelHeight = GetOwner()->HasFlag(KxDV_MODEL_ROW_HEIGHT);
	const bool variableHeight = GetOwner()->HasFlag(KxDV_VARIABLE_ROW_HEIGHT);

	if (modelHeight || variableHeight)
	{
		size_t columnCount = GetOwner()->GetColumnCount();
		int start = 0;

		for (size_t currentRow = 0; currentRow < row; currentRow++)
		{
			const KxDataViewTreeNode* node = GetTreeNodeByRow(currentRow);
			if (node)
			{
				int height = m_UniformLineHeight;

				KxDataViewItem item = node->GetItem();
				for (size_t currentColumn = 0; currentColumn < columnCount; currentColumn++)
				{
					const KxDataViewColumn* column = GetOwner()->GetColumn(currentColumn);
					if (column->IsVisible())
					{
						if ((currentColumn != 0) && model->IsContainer(item) &&	!model->HasContainerColumns(item))
						{
							continue;
						}

						if (modelHeight)
						{
							height = GetLineHeightModel(node);
						}
						else
						{
							KxDataViewRenderer* renderer = const_cast<KxDataViewRenderer*>(column->GetRenderer());
							renderer->PrepareItemToDraw(item, GetCellStateForRow(currentRow));
							height = std::max(height, renderer->GetCellSize().y);
						}
					}
				}
				start += height;
			}
			else
			{
				break;
			}
		}
		return start;
	}
	return row * m_UniformLineHeight;
}
int KxDataViewMainWindow::GetLineHeight(size_t row) const
{
	const KxDataViewModel* model = GetModel();
	const bool modelHeight = GetOwner()->HasFlag(KxDV_MODEL_ROW_HEIGHT);
	const bool variableHeight = GetOwner()->HasFlag(KxDV_VARIABLE_ROW_HEIGHT);

	if (modelHeight || variableHeight)
	{
		wxASSERT(!IsVirtualList());

		const KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		if (node)
		{
			KxDataViewItem item = node->GetItem();
			size_t columnCount = GetOwner()->GetColumnCount();

			int height = m_UniformLineHeight;
			for (size_t currentColumn = 0; currentColumn < columnCount; currentColumn++)
			{
				const KxDataViewColumn* column = GetOwner()->GetColumn(currentColumn);
				if (column->IsVisible())
				{
					if ((currentColumn != 0) && model->IsContainer(item) &&	!model->HasContainerColumns(item))
					{
						continue;
					}

					if (modelHeight)
					{
						height = GetLineHeightModel(node);
					}
					else
					{
						KxDataViewRenderer* renderer = const_cast<KxDataViewRenderer*>(column->GetRenderer());
						renderer->PrepareItemToDraw(item, GetCellStateForRow(row));
						height = std::max(height, renderer->GetCellSize().y);
					}
				}
			}
			return height;
		}
	}
	return m_UniformLineHeight;
}
int KxDataViewMainWindow::GetLineHeightModel(const KxDataViewTreeNode* node) const
{
	if (node)
	{
		int height = m_UniformLineHeight;
		if (GetModel()->GetCellHeight(node->GetItem(), height))
		{
			return height;
		}
	}
	return m_UniformLineHeight;
}
int KxDataViewMainWindow::GetLineHeightModel(size_t row) const
{
	return GetLineHeightModel(GetTreeNodeByRow(row));
}

size_t KxDataViewMainWindow::GetLineAt(int yCoord) const
{
	const KxDataViewModel* model = GetModel();
	const bool modelHeight = GetOwner()->HasFlag(KxDV_MODEL_ROW_HEIGHT);
	const bool variableHeight = GetOwner()->HasFlag(KxDV_VARIABLE_ROW_HEIGHT);

	if (modelHeight || variableHeight)
	{
		// TODO: make more efficient

		size_t row = 0;
		int yPos = 0;
		while (true)
		{
			const KxDataViewTreeNode* node = GetTreeNodeByRow(row);
			if (!node)
			{
				// Not really correct...
				return row + ((yCoord - yPos) / m_UniformLineHeight);
			}

			KxDataViewItem item = node->GetItem();
			size_t columnCount = GetOwner()->GetColumnCount();

			int height = m_UniformLineHeight;
			for (size_t currentColumn = 0; currentColumn < columnCount; currentColumn++)
			{
				const KxDataViewColumn* column = GetOwner()->GetColumn(currentColumn);
				if (column->IsVisible())
				{
					if ((currentColumn != 0) && model->IsContainer(item) &&	!model->HasContainerColumns(item))
					{
						continue;
					}

					if (modelHeight)
					{
						height = GetLineHeightModel(node);
					}
					else
					{
						KxDataViewRenderer* renderer = const_cast<KxDataViewRenderer*>(column->GetRenderer());
						renderer->PrepareItemToDraw(item, KxDATAVIEW_CELL_NONE);
						height = std::max(height, renderer->GetCellSize().y);
					}
				}
			}

			yPos += height;
			if (yCoord < yPos)
			{
				return row;
			}
			row++;
		}
	}
	return std::abs(yCoord) / m_UniformLineHeight;
}
int KxDataViewMainWindow::GetLineWidth() const
{
	return GetEndOfLastCol();
}

void KxDataViewMainWindow::SetUniformRowHeight(int height)
{
	m_UniformLineHeight = height > 0 ? height : GetDefaultRowHeight();
}
int KxDataViewMainWindow::GetDefaultRowHeight(KxDataViewDefaultRowHeightType type) const
{
	int resultHeight = 0;
	const int iconMargin = 6 * KxSystemSettings::GetMetric(wxSYS_BORDER_Y, this);
	const int iconHeight = KxSystemSettings::GetMetric(wxSYS_SMALLICON_Y, this);

	switch (type)
	{
		case KxDVC_ROW_HEIGHT_DEFAULT:
		{
			int userHeight = wxSystemOptions::GetOptionInt("KxDataViewCtrl::DefaultRowHeight");
			if (userHeight > 0)
			{
				return userHeight;
			}
			else
			{
				resultHeight = std::max(iconHeight + iconMargin, GetCharHeight() + iconMargin);
			}
			break;
		}
		case KxDVC_ROW_HEIGHT_LISTVIEW:
		{
			resultHeight = std::max(m_Owner->FromDIP(wxSize(-1, 17).y), GetCharHeight() + iconMargin);
			break;
		}
		case KxDVC_ROW_HEIGHT_EXPLORER:
		{
			resultHeight = std::max(m_Owner->FromDIP(wxSize(-1, 22).y), GetCharHeight() + iconMargin);
			break;
		}
	};
	return resultHeight >= iconHeight ? resultHeight : iconHeight;
}

/* Drag and Drop */
wxBitmap KxDataViewMainWindow::CreateItemBitmap(size_t row, int& indent)
{
	int height = GetLineHeight(row);
	int width = GetLineWidth();

	indent = 0;
	if (!IsList())
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		indent = GetOwner()->GetIndent() * node->GetIndentLevel();
		indent = indent + m_UniformLineHeight;

		// Try to use the 'm_UniformLineHeight' as the expander space
	}
	width -= indent;

	wxBitmap bitmap(width, height, 32);
	wxMemoryDC dc(bitmap);
	wxGCDC gcDC(dc);

	dc.SetFont(GetFont());
	dc.SetBackground(m_Owner->GetBackgroundColour());
	dc.SetTextBackground(m_Owner->GetBackgroundColour());
	dc.SetTextForeground(m_Owner->GetForegroundColour());
	dc.DrawRectangle(wxRect(0, 0, width, height));
	wxRendererNative::Get().DrawItemSelectionRect(this, dc, wxRect(0, 0, width, height), wxCONTROL_CURRENT|wxCONTROL_SELECTED|wxCONTROL_FOCUSED);

	KxDataViewColumn* expander = GetOwner()->GetExpanderColumnOrFirstOne();

	int x = 0;
	for (size_t columnIndex = 0; columnIndex < GetOwner()->GetColumnCount(); columnIndex++)
	{
		KxDataViewColumn* column = GetOwner()->GetColumnAt(columnIndex);
		KxDataViewRenderer* renderer = column->GetRenderer();

		if (column->IsExposed(width))
		{
			if (column == expander)
			{
				width -= indent;
			}

			KxDataViewCellState cellState = GetCellStateForRow(row);

			KxDataViewItem item = GetItemByRow(row);
			renderer->PrepareItemToDraw(item, cellState);

			wxRect itemRect(x, 0, width, height);
			itemRect.Deflate(PADDING_RIGHTLEFT, 0);

			renderer->SetDC(dc);
			renderer->SetGCDC(gcDC);
			renderer->CallDrawCellBackground(itemRect, cellState);
			renderer->CallDrawCellContent(itemRect, cellState);

			x += width;
		}
	}

	dc.SelectObject(wxNullBitmap);
	return bitmap;
}
bool KxDataViewMainWindow::EnableDragSource(const wxDataFormat& format)
{
	m_DragFormat = format;
	m_DragEnabled = format != wxDF_INVALID;

	return true;
}
bool KxDataViewMainWindow::EnableDropTarget(const wxDataFormat& format)
{
	m_DropFormat = format;
	m_DropEnabled = format != wxDF_INVALID;

	if (m_DropEnabled)
	{
		SetDropTarget(new KxDataViewMainWindowDropTarget(new wxCustomDataObject(format), this));
	}
	return true;
}

void KxDataViewMainWindow::OnDragDropGetRowItem(const wxPoint& pos, size_t& row, KxDataViewItem& item)
{
	// Get row
	wxPoint unscrolledPos;
	m_Owner->CalcUnscrolledPosition(pos.x, pos.y, &unscrolledPos.x, &unscrolledPos.y);
	row = GetLineAt(unscrolledPos.y);

	// Get item
	if (row < GetRowCount() && pos.y <= GetEndOfLastCol())
	{
		item = GetItemByRow(row);
	}
}
void KxDataViewMainWindow::RemoveDropHint()
{
	if (m_DropHint)
	{
		m_DropHint = false;
		RefreshRow(m_DropHintLine);
		m_DropHintLine = INVALID_ROW;
	}
}
wxDragResult KxDataViewMainWindow::OnDragOver(const wxDataFormat& format, const wxPoint& pos, wxDragResult dragResult)
{
	size_t row = INVALID_ROW;
	KxDataViewItem item;
	OnDragDropGetRowItem(pos, row, item);

	if (row != 0 && row < GetRowCount())
	{
		size_t firstVisible = GetFirstVisibleRow();
		size_t lastVisible = GetLastVisibleRow();
		if (row == firstVisible || row == firstVisible + 1)
		{
			ScrollTo(row - 1);
		}
		else if (row == lastVisible || row == lastVisible - 1)
		{
			ScrollTo(firstVisible + 1);
		}
	}

	KxDataViewEventDND event(KxEVT_DATAVIEW_ITEM_DROP_POSSIBLE);
	CreateEventTemplate(event, item);
	event.SetDataFormat(format);
	event.SetDropEffect(dragResult);

	if (!m_Owner->HandleWindowEvent(event) || !event.IsAllowed())
	{
		RemoveDropHint();
		return wxDragNone;
	}

	if (item.IsOK())
	{
		if (m_DropHint && (row != m_DropHintLine))
		{
			RefreshRow(m_DropHintLine);
		}
		m_DropHint = true;
		m_DropHintLine = row;
		RefreshRow(row);
	}
	else
	{
		RemoveDropHint();
	}
	return dragResult;
}
wxDragResult KxDataViewMainWindow::OnDragData(const wxDataFormat& format, const wxPoint& pos, wxDragResult dragResult)
{
	size_t row = INVALID_ROW;
	KxDataViewItem item;
	OnDragDropGetRowItem(pos, row, item);

	wxCustomDataObject* dataObject = (wxCustomDataObject*)GetDropTarget()->GetDataObject();

	KxDataViewEventDND event(KxEVT_DATAVIEW_ITEM_DROP);
	CreateEventTemplate(event, item);
	event.SetDataFormat(format);
	event.SetDataSize(dataObject->GetSize());
	event.SetDataBuffer(dataObject->GetData());
	event.SetDropEffect(dragResult);

	if (!m_Owner->HandleWindowEvent(event) || !event.IsAllowed())
	{
		return wxDragNone;
	}
	return dragResult;
}
bool KxDataViewMainWindow::OnDrop(const wxDataFormat& format, const wxPoint& pos)
{
	RemoveDropHint();

	size_t row = INVALID_ROW;
	KxDataViewItem item;
	OnDragDropGetRowItem(pos, row, item);

	KxDataViewEventDND event(KxEVT_DATAVIEW_ITEM_DROP_POSSIBLE);
	CreateEventTemplate(event, item);
	event.SetDataFormat(format);

	if (!m_Owner->HandleWindowEvent(event) || !event.IsAllowed())
	{
		return false;
	}
	return true;
}
void KxDataViewMainWindow::OnDragDropLeave()
{
	RemoveDropHint();
}

/* Scrolling */
void KxDataViewMainWindow::ScrollWindow(int dx, int dy, const wxRect* rect)
{
	m_TreeNodeUnderMouse = nullptr;
	wxWindow::ScrollWindow(dx, dy, rect);

	if (wxHeaderCtrl* header = GetOwner()->GetHeaderCtrl())
	{
		header->ScrollWindow(dx, 0);
	}
}
void KxDataViewMainWindow::ScrollTo(size_t row, size_t column)
{
	m_TreeNodeUnderMouse = nullptr;

	wxPoint pos;
	m_Owner->GetScrollPixelsPerUnit(&pos.x, &pos.y);
	wxPoint scrollPos(-1, GetLineStart(row) / pos.y);

	if (column != INVALID_COLUMN)
	{
		wxRect rect = GetClientRect();

		wxPoint unscrolledPos;
		m_Owner->CalcUnscrolledPosition(rect.x, rect.y, &unscrolledPos.x, &unscrolledPos.y);

		int columnWidth = 0;
		int x_start = 0;
		for (size_t colnum = 0; colnum < column; colnum++)
		{
			if (GetOwner()->GetColumnAt(colnum)->IsExposed(columnWidth))
			{
				x_start += columnWidth;
			}
		}

		int x_end = x_start + columnWidth;
		int xe = unscrolledPos.x + rect.width;
		if (x_end > xe && scrollPos.x != 0)
		{
			scrollPos.x = (unscrolledPos.x + x_end - xe) / scrollPos.x;
		}
		if (x_start < unscrolledPos.x && scrollPos.x)
		{
			scrollPos.x = x_start / scrollPos.x;
		}
	}
	m_Owner->Scroll(scrollPos);
}

/* Current row and column */
void KxDataViewMainWindow::ChangeCurrentRow(size_t row)
{
	m_CurrentRow = row;
	// Send event ?
}
bool KxDataViewMainWindow::TryAdvanceCurrentColumn(KxDataViewTreeNode* node, wxKeyEvent& event, bool moveForward)
{
	if (GetOwner()->GetColumnCount() == 0)
	{
		return false;
	}

	if (!m_UseCellFocus)
	{
		return false;
	}
	const bool wrapAround = event.GetKeyCode() == WXK_TAB;

	if (node)
	{
		// Navigation shouldn't work in branch nodes without other columns:
		if (node->HasChildren() && !GetModel()->HasContainerColumns(node->GetItem()))
		{
			return false;
		}
	}

	if (m_CurrentColumn == nullptr || !m_IsCurrentColumnSetByKeyboard)
	{
		if (moveForward)
		{
			m_CurrentColumn = GetOwner()->GetColumnAt(1);
			m_IsCurrentColumnSetByKeyboard = true;
			RefreshRow(m_CurrentRow);
			return true;
		}
		else
		{
			if (!wrapAround)
			{
				return false;
			}
		}
	}

	size_t nextColumn = GetOwner()->GetColumnIndex(m_CurrentColumn) + (moveForward ? +1 : -1);
	if (nextColumn >= GetOwner()->GetColumnCount())
	{
		if (!wrapAround)
		{
			return false;
		}

		if (GetCurrentRow() < GetRowCount() - 1)
		{
			// Go to the first column of the next row:
			nextColumn = 0;
			OnVerticalNavigation(wxKeyEvent()/*dummy*/, +1);
		}
		else
		{
			// allow focus change
			event.Skip();
			return false;
		}
	}

	if (nextColumn == 0 && wrapAround)
	{
		if (GetCurrentRow() > 0)
		{
			// Go to the last column of the previous row
			nextColumn = GetOwner()->GetColumnCount() - 1;
			OnVerticalNavigation(wxKeyEvent(), -1);
		}
		else
		{
			// Allow focus change
			event.Skip();
			return false;
		}
	}

	GetOwner()->EnsureVisible(m_CurrentRow, nextColumn);

	if (nextColumn < 1)
	{
		// We are going to the left of the second column. Reset to whole-row
		// focus (which means first column would be edited).
		m_CurrentColumn = nullptr;
		RefreshRow(m_CurrentRow);
		return true;
	}

	m_CurrentColumn = GetOwner()->GetColumnAt(nextColumn);
	m_IsCurrentColumnSetByKeyboard = true;
	RefreshRow(m_CurrentRow);
	return true;
}

KxDataViewItem KxDataViewMainWindow::GetHotTrackItem() const
{
	if (m_HotTrackRow != INVALID_ROW)
	{
		return GetItemByRow(m_HotTrackRow);
	}
	return KxDataViewItem();
}
KxDataViewColumn* KxDataViewMainWindow::GetHotTrackColumn() const
{
	return m_HotTrackColumn;
}

/* Selection */
bool KxDataViewMainWindow::UnselectAllRows(size_t exceptThisRow)
{
	if (!IsSelectionEmpty())
	{
		for (size_t i = GetFirstVisibleRow(); i <= GetLastVisibleRow(); i++)
		{
			if (m_Selection.IsSelected(i) && i != exceptThisRow)
			{
				RefreshRow(i);
			}
		}

		if (exceptThisRow != INVALID_ROW)
		{
			const bool wasSelected = m_Selection.IsSelected(exceptThisRow);
			ClearSelection();
			if (wasSelected)
			{
				m_Selection.SelectItem(exceptThisRow);

				// The special item is still selected.
				return false;
			}
		}
		else
		{
			ClearSelection();
		}
	}

	// There are no selected items left.
	return true;
}

void KxDataViewMainWindow::SelectRow(size_t row, bool select)
{
	if (m_Selection.SelectItem(row, select))
	{
		RefreshRow(row);
	}
}
void KxDataViewMainWindow::SelectRows(size_t from, size_t to)
{
	wxArrayInt changed;
	if (m_Selection.SelectRange(from, to, true, &changed))
	{
		for (size_t i = 0; i < changed.size(); i++)
		{
			RefreshRow(changed[i]);
		}
	}
	else
	{
		// Selection of too many rows has changed.
		RefreshRows(from, to);
	}
}
void KxDataViewMainWindow::SelectRows(const KxIntPtrVector& selection)
{
	for (size_t row: selection)
	{
		if (m_Selection.SelectItem(row))
		{
			RefreshRow(row);
		}
	}
}
void KxDataViewMainWindow::ReverseRowSelection(size_t row)
{
	m_Selection.SelectItem(row, !IsRowSelected(row));
	RefreshRow(row);
}

/* View */
size_t KxDataViewMainWindow::GetCountPerPage() const
{
	wxSize size = GetClientSize();
	return size.y / m_UniformLineHeight;
}
int KxDataViewMainWindow::GetEndOfLastCol() const
{
	int width = 0;
	for (size_t i = 0; i < GetOwner()->GetColumnCount(); i++)
	{
		int widthThis = 0;
		if (GetOwner()->GetColumn(i)->IsExposed(widthThis))
		{
			width += widthThis;
		}
	}
	return width;
}
size_t KxDataViewMainWindow::GetFirstVisibleRow() const
{
	wxPoint pos(0, 0);
	m_Owner->CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);
	return GetLineAt(pos.y);
}
size_t KxDataViewMainWindow::GetLastVisibleRow() const
{
	wxSize size = GetClientSize();
	m_Owner->CalcUnscrolledPosition(size.x, size.y, &size.x, &size.y);

	// We should deal with the pixel here.
	size_t row = GetLineAt(size.y) - 1;
	return std::min(GetRowCount() - 1, row);
}
size_t KxDataViewMainWindow::GetRowCount() const
{
	if (m_ItemsCount == INVALID_COUNT)
	{
		KxDataViewMainWindow* self = const_cast<KxDataViewMainWindow*>(this);
		self->UpdateItemCount(RecalculateItemCount());
		self->UpdateDisplay();
	}
	return m_ItemsCount;
}

void KxDataViewMainWindow::HitTest(const wxPoint& pos, KxDataViewItem& item, KxDataViewColumn*& column)
{
	KxDataViewColumn* columnFound = nullptr;
	size_t columnCount = GetOwner()->GetColumnCount();

	wxPoint unscrolledPos;
	m_Owner->CalcUnscrolledPosition(pos.x, pos.y, &unscrolledPos.x, &unscrolledPos.y);

	int x_start = 0;
	for (size_t colnumIndex = 0; colnumIndex < columnCount; colnumIndex++)
	{
		KxDataViewColumn* testColumn = GetOwner()->GetColumnAt(colnumIndex);

		int width = 0;
		if (testColumn->IsExposed(width))
		{
			if (x_start + width >= unscrolledPos.x)
			{
				columnFound = testColumn;
				break;
			}
			x_start += width;
		}
	}

	column = columnFound;
	item = GetItemByRow(GetLineAt(unscrolledPos.y));
}
wxRect KxDataViewMainWindow::GetItemRect(const KxDataViewItem& item, const KxDataViewColumn* column)
{
	int xpos = 0;
	int width = 0;
	size_t columnCount = GetOwner()->GetColumnCount();

	// If column is null the loop will compute the combined width of all columns.
	// Otherwise, it will compute the x position of the column we are looking for.
	for (size_t i = 0; i < columnCount; i++)
	{
		KxDataViewColumn* currentColumn = GetOwner()->GetColumnAt(i);
		if (currentColumn == column)
		{
			break;
		}

		int widthCurrent = 0;
		if (currentColumn->IsExposed(widthCurrent))
		{
			xpos += widthCurrent;
			width += widthCurrent;
		}
	}

	if (column)
	{
		// If we have a column, we need can get its width directly.
		width = 0;
		column->IsExposed(width);
	}
	else
	{
		// If we have no column, we reset the x position back to zero.
		xpos = 0;
	}

	size_t row = GetRowByItem(item);
	if (row == INVALID_ROW)
	{
		// This means the row is currently not visible at all.
		return wxRect();
	}

	// We have to take an expander column into account and compute its indentation
	// to get the correct x position where the actual text is.
	int indent = 0;
	if (!IsList() && (!column || m_Owner->GetExpanderColumnOrFirstOne() == column))
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		indent = GetOwner()->GetIndent() * node->GetIndentLevel();
		
		// Use 'm_UniformLineHeight' as the width of the expander
		indent += m_UniformLineHeight;
	}

	wxRect itemRect(xpos + indent, GetLineStart(row), width - indent, GetLineHeight(row));
	GetOwner()->CalcScrolledPosition(itemRect.x, itemRect.y, &itemRect.x, &itemRect.y);
	return itemRect;
}

void KxDataViewMainWindow::UpdateColumnSizes()
{
	size_t columnCount = GetOwner()->GetVisibleColumnCount();
	if (columnCount != 0)
	{
		KxDataViewColumn* lastColumn = m_Owner->GetColumnAtVisible(columnCount - 1);
		if (lastColumn)
		{
			const bool autoResize = !m_Owner->HasFlag(KxDV_NO_COLUMN_AUTO_SIZE) || columnCount <= 1;
			const int fullWinWidth = GetSize().x;
			const int columnsFullWidth = GetEndOfLastCol();
			const int lastColumnLeft = columnsFullWidth - lastColumn->GetWidth();

			if (autoResize && lastColumnLeft < fullWinWidth)
			{
				int desiredWidth = std::max(fullWinWidth - lastColumnLeft, lastColumn->GetMinWidth());
				if (desiredWidth < (int)GetBestColumnWidth(columnCount - 1))
				{
					lastColumn->SetWidth(lastColumn->GetWidth());
					SetVirtualSize(columnsFullWidth, m_virtualSize.y);
					return;
				}
				lastColumn->SetWidth(desiredWidth);

				// All columns fit on screen, so we don't need horizontal scrolling.
				// To prevent flickering scrollbar when resizing the window to be
				// narrower, force-set the virtual width to 0 here. It will eventually
				// be corrected at idle time.
				SetVirtualSize(0, m_virtualSize.y);
				RefreshRect(wxRect(lastColumnLeft, 0, fullWinWidth - lastColumnLeft, GetSize().y));
			}
			else
			{
				if (!autoResize)
				{
					lastColumn->SetWidth(lastColumn->GetWidth());
				}

				// Don't bother, the columns won't fit anyway
				SetVirtualSize(columnsFullWidth, m_virtualSize.y);
			}
		}
	}
}

/* Rows */
void KxDataViewMainWindow::Expand(size_t row)
{
	if (!IsList())
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		if (node && node->HasChildren() && !node->IsExpanded())
		{
			if (!SendExpanderEvent(KxEVT_DATAVIEW_ITEM_EXPANDING, node->GetItem()))
			{
				// Vetoed by the event handler.
				return;
			}
			node->ToggleExpanded(this);

			// Build the children of current node
			if (node->GetChildNodes().empty())
			{
				BuildTreeHelper(node->GetItem(), node);
			}

			const ptrdiff_t countNewRows = node->GetSubTreeCount();

			// Shift all stored indices after this row by the number of newly added rows.
			m_Selection.OnItemsInserted(row + 1, countNewRows);
			if (m_CurrentRow > row)
			{
				ChangeCurrentRow(m_CurrentRow + countNewRows);
			}

			if (m_ItemsCount != INVALID_COUNT)
			{
				m_ItemsCount += countNewRows;
			}

			// Expanding this item means the previously cached column widths could
			// have become invalid as new items are now visible.
			GetOwner()->InvalidateColBestWidths();
			UpdateDisplay();

			// Send the expanded event
			SendExpanderEvent(KxEVT_DATAVIEW_ITEM_EXPANDED, node->GetItem());
		}
	}
}
void KxDataViewMainWindow::Collapse(size_t row)
{
	if (!IsList())
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		if (node && node->HasChildren() && node->IsExpanded())
		{
			if (!SendExpanderEvent(KxEVT_DATAVIEW_ITEM_COLLAPSING, node->GetItem()))
			{
				// Vetoed by the event handler.
				return;
			}

			ptrdiff_t countDeletedRows = node->GetSubTreeCount();
			if (m_Selection.OnItemsDeleted(row + 1, countDeletedRows))
			{
				RefreshRow(row);
				SendSelectionChangedEvent(row, m_CurrentColumn);
			}
			node->ToggleExpanded(this);

			// Adjust the current row if necessary.
			if (m_CurrentRow > row)
			{
				// If the current row was among the collapsed items, make the
				// parent itself current.
				if (m_CurrentRow <= row + countDeletedRows)
				{
					ChangeCurrentRow(row);
				}
				else
				{
					// Otherwise just update the index.
					ChangeCurrentRow(m_CurrentRow - countDeletedRows);
				}
			}

			if (m_ItemsCount != INVALID_COUNT)
			{
				m_ItemsCount -= countDeletedRows;
			}

			GetOwner()->InvalidateColBestWidths();
			UpdateDisplay();

			SendExpanderEvent(KxEVT_DATAVIEW_ITEM_COLLAPSED, node->GetItem());
		}
	}
}
bool KxDataViewMainWindow::IsExpanded(size_t row) const
{
	if (!IsList())
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		if (node && node->HasChildren())
		{
			return node->IsExpanded();
		}
	}
	return false;
}
bool KxDataViewMainWindow::HasChildren(size_t row) const
{
	if (!IsList())
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		return node && node->HasChildren();
	}
	return false;
}

KxDataViewItem KxDataViewMainWindow::GetItemByRow(size_t row) const
{
	if (const KxDataViewVirtualListModel* modelVirtualList = GetVirtualListModel())
	{
		if (row < GetRowCount())
		{
			return modelVirtualList->GetItem(row);
		}
	}
	else
	{
		KxDataViewTreeNode* node = GetTreeNodeByRow(row);
		if (node)
		{
			return node->GetItem();
		}
	}
	return KxDataViewItem();
}
size_t KxDataViewMainWindow::GetRowByItem(const KxDataViewItem& item) const
{
	if (const KxDataViewVirtualListModel* modelVirtualList = GetVirtualListModel())
	{
		return modelVirtualList->GetRow(item);
	}
	else if (GetModel() && item.IsOK())
	{
		// Compose the parent-chain of the item we are looking for
		KxDataViewItem::Vector parentChain;
		KxDataViewItem it(item);
		while (it.IsOK())
		{
			parentChain.push_back(it);
			it = GetModel()->GetParent(it);
		}

		// Add an 'invalid' item to represent our 'invisible' root node
		parentChain.push_back(KxDataViewItem());

		// The parent chain was created by adding the deepest parent first.
		// so if we want to start at the root node, we have to iterate backwards through the vector
		KxDataViewTreeNodeOperation_ItemToRow operation(item, parentChain.rbegin());

		// If the item was not found at all, which can happen if all its parents
		// are not expanded, this function still returned a valid but completely
		// wrong row index.
		//
		// This affected many functions which could call it for the items which
		// were not necessarily visible, i.e. all of them except for the event
		// handlers as events can only affect the visible items, including but not
		// limited to SetCurrentItem(), all the selection-related functions, all
		// the expansion-related functions, EnsureVisible(), HitTest() and
		// GetItemRect().
		if (operation.Walk(m_TreeRoot))
		{
			return operation.GetResult();
		}
	}
	return INVALID_ROW;
}

bool KxDataViewMainWindow::IsCellEditable(const KxDataViewItem& item, const KxDataViewColumn* column, KxDataViewCellMode mode) const
{
	switch (mode)
	{
		case KxDATAVIEW_CELL_EDITABLE:
		{
			return column->IsEditable() && GetModel()->IsEditorEnabled(item, column);
		}
		case KxDATAVIEW_CELL_ACTIVATABLE:
		{
			return column->IsActivatable() && GetModel()->IsEnabled(item, column);
		}
		case KxDATAVIEW_CELL_ANY:
		{
			bool editorEnabled = GetModel()->IsEditorEnabled(item, column);
			bool cellEnabled = GetModel()->IsEnabled(item, column);
			return (column->IsEditable() && editorEnabled) || (column->IsActivatable() && cellEnabled);
		}
	};
	return false;
}
bool KxDataViewMainWindow::BeginEdit(const KxDataViewItem& item, const KxDataViewColumn* column)
{
	// Cancel any previous editing
	CancelEdit();

	KxDataViewEvent event(KxEVT_DATAVIEW_ITEM_EDIT_ATTACH);
	CreateEventTemplate(event, item, const_cast<KxDataViewColumn*>(column));
	m_Owner->ProcessWindowEvent(event);

	if (IsCellEditable(item, column, KxDATAVIEW_CELL_EDITABLE))
	{
		KxDataViewEditor* editor = column->GetEditor();
		if (editor)
		{
			GetOwner()->EnsureVisible(item, column);
			GetOwner()->SetFocus();

			const wxRect itemRect = GetItemRect(item, column);
			if (editor->BeginEdit(item, itemRect))
			{
				// Save the renderer to be able to finish/cancel editing it later.
				m_CurrentEditor = editor;
				return true;
			}
		}
	}
	return false;
}
void KxDataViewMainWindow::EndEdit()
{
	if (m_CurrentEditor)
	{
		KxDataViewEvent event(KxEVT_DATAVIEW_ITEM_EDIT_DETACH);
		CreateEventTemplate(event, m_CurrentEditor->GetItem(), m_CurrentEditor->GetColumn());
		
		m_CurrentEditor->EndEdit();
		m_CurrentEditor = nullptr;

		m_Owner->ProcessWindowEvent(event);
	}
}
void KxDataViewMainWindow::CancelEdit()
{
	if (m_CurrentEditor)
	{
		KxDataViewEvent event(KxEVT_DATAVIEW_ITEM_EDIT_DETACH);
		CreateEventTemplate(event, m_CurrentEditor->GetItem(), m_CurrentEditor->GetColumn());

		m_CurrentEditor->CancelEdit();
		m_CurrentEditor = nullptr;

		m_Owner->ProcessWindowEvent(event);
	}
}

//////////////////////////////////////////////////////////////////////////
#include <wx/dragimag.h>
#include <wx/minifram.h>

void KxDataViewMainWindowDropSource::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(m_DragImage);
	dc.DrawBitmap(m_HintBitmap, 0, 0);
}
void KxDataViewMainWindowDropSource::OnScroll(wxMouseEvent& event)
{
	if (m_MainWindow)
	{
		KxDataViewCtrl* view = m_MainWindow->GetOwner();

		int rateX = 0;
		int rateY = 0;
		view->GetScrollPixelsPerUnit(&rateX, &rateY);
		wxPoint startPos = view->GetViewStart();

		wxCoord value = -event.GetWheelRotation();
		if (event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
		{
			view->Scroll(wxDefaultCoord, startPos.y + (float)value / (rateY != 0 ? rateY : 1));
		}
		else
		{
			view->Scroll(startPos.x + (float)value / (rateX != 0 ? rateX : 1), wxDefaultCoord);
		}
	}
	event.Skip();
}
bool KxDataViewMainWindowDropSource::GiveFeedback(wxDragResult effect)
{
	wxPoint mousePos = wxGetMousePosition();

	if (!m_DragImage)
	{
		wxPoint linePos(0, m_MainWindow->GetLineStart(m_Row));

		m_MainWindow->GetOwner()->CalcUnscrolledPosition(0, linePos.y, nullptr, &linePos.y);
		m_MainWindow->ClientToScreen(&linePos.x, &linePos.y);

		m_Distance.x = mousePos.x - linePos.x;
		m_Distance.y = mousePos.y - linePos.y;

		int rowIndent = 0;
		m_HintBitmap = m_MainWindow->CreateItemBitmap(m_Row, rowIndent);

		m_Distance.x -= rowIndent;
		m_HintPosition = GetHintPosition(mousePos);

		int frameStyle = wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxNO_BORDER;
		m_DragImage = new wxMiniFrame(m_MainWindow, wxID_NONE, wxEmptyString, m_HintPosition, m_HintBitmap.GetSize(), frameStyle);
		//m_DragImage = new wxPopupWindow(m_MainWindow);
		//m_DragImage->SetInitialSize(m_HintBitmap.GetSize());

		m_DragImage->Bind(wxEVT_PAINT, &KxDataViewMainWindowDropSource::OnPaint, this);
		m_DragImage->SetTransparent(128);
		m_DragImage->Show();
	}
	else
	{
		m_HintPosition = GetHintPosition(mousePos);
		m_DragImage->Move(m_HintPosition);
	}
	return false;
}

wxPoint KxDataViewMainWindowDropSource::GetHintPosition(const wxPoint& mousePos) const
{
	return wxPoint(mousePos.x - m_Distance.x, mousePos.y + 5);
}

KxDataViewMainWindowDropSource::KxDataViewMainWindowDropSource(KxDataViewMainWindow* mainWindow, size_t row)
	:wxDropSource(mainWindow), m_MainWindow(mainWindow), m_Row(row), m_Distance(0, 0)
{
	//m_MainWindow->Bind(wxEVT_MOUSEWHEEL, &KxDataViewMainWindowDropSource::OnScroll, this);
}
KxDataViewMainWindowDropSource::~KxDataViewMainWindowDropSource()
{
	m_HintPosition = wxDefaultPosition;
	delete m_DragImage;
}

//////////////////////////////////////////////////////////////////////////
wxDragResult KxDataViewMainWindowDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult dragResult)
{
	wxDataFormat format = GetMatchingPair();
	if (format != wxDF_INVALID)
	{
		return m_MainWindow->OnDragOver(format, wxPoint(x, y), dragResult);
	}
	return wxDragNone;
}
bool KxDataViewMainWindowDropTarget::OnDrop(wxCoord x, wxCoord y)
{
	wxDataFormat format = GetMatchingPair();
	if (format != wxDF_INVALID)
	{
		return m_MainWindow->OnDrop(format, wxPoint(x, y));
	}
	return false;
}
wxDragResult KxDataViewMainWindowDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult dragResult)
{
	wxDataFormat format = GetMatchingPair();
	if (format == wxDF_INVALID || !GetData())
	{
		return wxDragNone;
	}
	else
	{
		return m_MainWindow->OnDragData(format, wxPoint(x, y), dragResult);
	}
}
void KxDataViewMainWindowDropTarget::OnLeave()
{
	m_MainWindow->OnDragDropLeave();
}

KxDataViewMainWindowDropTarget::KxDataViewMainWindowDropTarget(wxDataObject* dataObject, KxDataViewMainWindow* mainWindow)
	:wxDropTarget(dataObject), m_MainWindow(mainWindow)
{
}
