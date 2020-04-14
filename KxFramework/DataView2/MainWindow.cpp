#include "KxStdAfx.h"
#include "MainWindow.h"
#include "HeaderCtrl.h"
#include "Renderer.h"
#include "ToolTip.h"
#include "Editor.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "KxFramework/KxDataView2Event.h"
#include "KxFramework/KxSplashWindow.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxFrame.h"
#include "Kx/System/SystemInformation.h"
#include "Kx/Drawing/UxTheme.h"
#include "Kx/Drawing/Private/UxThemeDefines.h"
#include "Kx/Drawing/DCOperations.h"
#include "Kx/Drawing/GCOperations.h"
#include <wx/popupwin.h>
#include <wx/generic/private/widthcalc.h>
#include <wx/minifram.h>
#include <wx/rawbmp.h>

namespace
{
	enum: int
	{
		// Cell padding on the left/right
		PADDING_RIGHTLEFT = 3,

		// Expander space margin
		EXPANDER_MARGIN = 2,
		EXPANDER_OFFSET = 2,
	};
}

namespace KxDataView2
{
	class MaxWidthCalculator: public wxMaxWidthCalculatorBase
	{
		private:
			MainWindow* m_MainWindow = nullptr;
			Column& m_Column;

			bool m_IsExpanderColumn = false;
			int m_ExpanderSize = 0;

		protected:
			void UpdateWithRow(int row) override
			{
				Node* node = m_MainWindow->GetNodeByRow(row);
				if (node)
				{
					int indent = 0;
					if (m_IsExpanderColumn)
					{
						indent = m_MainWindow->m_Indent * node->GetIndentLevel() + m_ExpanderSize;
					}

					Renderer& renderer = node->GetRenderer(m_Column);
					renderer.BeginCellSetup(*node, m_Column);
					renderer.SetupCellValue();
					renderer.SetupCellAttributes(m_MainWindow->GetCellStateForRow(row));
					UpdateWithWidth(renderer.GetCellSize().x + indent);
					renderer.EndCellSetup();
				}
			}

		public:
			MaxWidthCalculator(MainWindow* mainWindow, Column& column, int expanderSize)
				:wxMaxWidthCalculatorBase(column.GetDisplayIndex()), m_MainWindow(mainWindow), m_Column(column), m_ExpanderSize(expanderSize)
			{
				m_IsExpanderColumn = !m_MainWindow->IsList() && m_MainWindow->m_View->GetExpanderColumnOrFirstOne() == &m_Column;
			}
	};
}

namespace KxDataView2
{
	wxIMPLEMENT_ABSTRACT_CLASS(MainWindow, wxWindow);

	// Events
	void MainWindow::OnChar(wxKeyEvent& event)
	{
		// Propagate the char event upwards
		wxKeyEvent eventForParent(event);
		eventForParent.SetEventObject(m_View);
		if (m_View->ProcessWindowEvent(eventForParent))
		{
			return;
		}
		if (m_View->HandleAsNavigationKey(event))
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
					// Enter activates the item, i.e. sends EvtITEM_ACTIVATED to it.
					// Only if that event is not handled do we activate column renderer (which
					// is normally done by Space) or even inline editing.
					Event evt(EvtITEM_ACTIVATED);
					CreateEventTemplate(evt, GetNodeByRow(m_CurrentRow));

					if (m_View->ProcessWindowEvent(evt))
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
					Node* node = GetNodeByRow(m_CurrentRow);

					// Activate the current activatable column. If not column is focused (typically
					// because the user has full row selected), try to find the first activatable
					// column (this would typically be a checkbox and we don't want to force the user
					// to set focus on the checkbox column).
					Column* activatableColumn = FindInteractibleColumn(*node, InteractibleCell::Activator);
					if (activatableColumn)
					{
						const wxRect cellRect = m_View->GetItemRect(*node, activatableColumn);

						Renderer& renderer = node->GetRenderer(*activatableColumn);
						renderer.BeginCellSetup(*node, *activatableColumn);
						renderer.SetupCellAttributes(GetCellStateForRow(m_CurrentRow));
						renderer.CallOnActivateCell(*node, cellRect, nullptr);
						renderer.EndCellSetup();
						break;
					}
					break;
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
					if (!m_SelectionStore.IsEmpty())
					{
						// Mimic Windows 7 behavior: edit the item that has focus
						// if it is selected and the first selected item if focus
						// is out of selection.
						Row selectedRow;
						if (m_SelectionStore.IsSelected(m_CurrentRow))
						{
							selectedRow = m_CurrentRow;
						}
						else
						{
							// Focused item is not selected.
							wxSelectionStore::IterationState cookie;
							selectedRow = m_SelectionStore.GetFirstSelectedItem(cookie);
						}
						Node* node = GetNodeByRow(selectedRow);

						// Edit the current column. If no column is focused (typically because the user has full row selected),
						// try to find the first editable column.
						if (Column* editableColumn = FindInteractibleColumn(*node, InteractibleCell::Editor))
						{
							m_View->EditItem(*node, *editableColumn);
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
	void MainWindow::OnCharHook(wxKeyEvent& event)
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
					[[fallthrough]];
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
	void MainWindow::OnVerticalNavigation(const wxKeyEvent& event, int delta)
	{
		// If there is no selection, we cannot move it anywhere
		if (!HasCurrentRow() || IsEmpty())
		{
			return;
		}

		// Let's keep the new row inside the allowed range
		intptr_t newRow = (intptr_t)m_CurrentRow + delta;
		if (newRow < 0)
		{
			newRow = 0;
		}

		size_t rowCount = GetRowCount();
		if (newRow >= (intptr_t)rowCount)
		{
			newRow = rowCount - 1;
		}

		Row oldCurrent = m_CurrentRow;
		Row newCurrent = newRow;
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
			auto firstSel = m_SelectionStore.GetFirstSelectedItem(cookie);
			if (firstSel != wxSelectionStore::NO_SELECTION)
			{
				SendSelectionChangedEvent(GetNodeByRow(firstSel), m_CurrentColumn);
			}
		}
		else
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
				SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), m_CurrentColumn);
			}
			else
			{
				RefreshRow(m_CurrentRow);
			}
		}
		EnsureVisible(m_CurrentRow);
	}
	void MainWindow::OnLeftKey(wxKeyEvent& event)
	{
		if (IsList())
		{
			TryAdvanceCurrentColumn(nullptr, event, false);
		}
		else
		{
			Node* node = GetNodeByRow(m_CurrentRow);
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

				// Allow focus change
				event.Skip();
				return;
			}

			// Because TryAdvanceCurrentColumn() return false, we are at the first
			// column or using whole-row selection. In this situation, we can use
			// the standard TreeView handling of the left key.
			if (node->HasChildren() && node->IsNodeExpanded())
			{
				Collapse(m_CurrentRow);
			}
			else
			{
				// If the node is already closed, we move the selection to its parent
				Node* parentNode = node->GetParent();
				if (parentNode)
				{
					Row parent = GetRowByNode(*parentNode);
					if (parent)
					{
						SelectRow(m_CurrentRow, false);
						SelectRow(parent, true);
						ChangeCurrentRow(parent);
						EnsureVisible(parent);
						SendSelectionChangedEvent(parentNode, m_CurrentColumn);
					}
				}
			}
		}
	}
	void MainWindow::OnRightKey(wxKeyEvent& event)
	{
		if (IsList())
		{
			TryAdvanceCurrentColumn(nullptr, event, true);
		}
		else if (Node* node = GetNodeByRow(m_CurrentRow))
		{
			if (node->HasChildren() && m_CurrentColumn && m_CurrentColumn->IsExpander())
			{
				if (!node->IsNodeExpanded())
				{
					Expand(m_CurrentRow);
				}
				else
				{
					// If the node is already open, we move the selection to the first child
					SelectRow(m_CurrentRow, false);
					SelectRow(m_CurrentRow + 1, true);
					ChangeCurrentRow(m_CurrentRow + 1);
					EnsureVisible(m_CurrentRow + 1);
					SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow + 1), m_CurrentColumn);
				}
				return;
			}
			TryAdvanceCurrentColumn(node, event, true);
		}
	}

	void MainWindow::OnMouse(wxMouseEvent& event)
	{
		auto ResetHotTrackedExpander = [this]()
		{
			if (m_TreeNodeUnderMouse)
			{
				Row row = GetRowByNode(*m_TreeNodeUnderMouse);
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
			wxSize size = m_View->GetClientSize();

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
					const int scrollPos = m_View->GetScrollPos(wxVERTICAL);
					if (scrollPos > 0 && scrollPos + m_View->GetScrollPageSize(wxVERTICAL) < m_View->GetScrollLines(wxVERTICAL))
					{
						int rateX = 0;
						int rateY = 0;
						m_View->GetScrollPixelsPerUnit(&rateX, &rateY);

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
		m_View->CalcUnscrolledPosition(x, y, &x, &y);
		Column* currentColumn = nullptr;

		int xpos = 0;
		const size_t columnsCount = m_View->GetColumnCount();
		for (size_t i = 0; i < columnsCount; i++)
		{
			Column* column = m_View->GetColumnDisplayedAt(i);

			int width = 0;
			if (column->IsExposed(width))
			{
				if (x < xpos + width)
				{
					currentColumn = column;
					break;
				}
				xpos += width;
			}
		}

		const Row currentRow = GetRowAt(y);
		Node* const currentNode = GetNodeByRow(currentRow);

		// Hot track
		if (isMouseInsideWindow)
		{
			const bool rowChnaged = (m_HotTrackRow != currentRow && currentNode) || (m_HotTrackRow && currentNode == nullptr);
			const bool columnChanged = m_HotTrackColumn != currentColumn;

			if (rowChnaged || columnChanged)
			{
				m_HotTrackColumn = currentColumn;

				// Refresh old hot-tracked row
				if (rowChnaged)
				{
					m_HotTrackRowEnabled = false;
					RefreshRow(m_HotTrackRow);
				}
				if (currentNode == nullptr)
				{
					ResetHotTrackedExpander();
				}

				// Update new row
				m_HotTrackRow = currentNode ? currentRow : Row();
				m_HotTrackRowEnabled = m_HotTrackRow && m_HotTrackColumn;
				RefreshRow(m_HotTrackRow);

				Event hoverEvent(EvtITEM_HOVERED);
				CreateEventTemplate(hoverEvent, currentNode, m_HotTrackColumn);
				m_View->ProcessWindowEvent(hoverEvent);

				// Show tooltip
				RemoveTooltip();
				if (m_HotTrackRow && m_HotTrackColumn)
				{
					m_ToolTipTimer.StartOnce(wxSystemSettings::GetMetric(wxSYS_DCLICK_MSEC));
				}
			}
		}

		// Handle right clicking here, before everything else as context menu events should be
		// sent even when we click outside of any item, unlike all the other ones.
		if (event.RightUp())
		{
			CancelEdit();

			Event evt(EvtITEM_CONTEXT_MENU);
			CreateEventTemplate(evt, currentNode, currentColumn);
			m_View->ProcessWindowEvent(evt);
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
			if (m_DragCount < 3 && event.Leaving())
			{
				m_DragCount = 3;
			}
			else if (m_DragCount != 3)
			{
				return;
			}

			if (event.LeftIsDown())
			{
				m_View->CalcUnscrolledPosition(m_DragStart.x, m_DragStart.y, &m_DragStart.x, &m_DragStart.y);
				const Row draggedRow = GetRowAt(m_DragStart.y);
				Node* const draggedNode = GetNodeByRow(draggedRow);

				// Don't allow invalid items
				if (draggedNode)
				{
					RemoveTooltip();

					// Notify cell about drag
					EventDND dragEvent(EvtITEM_DRAG);
					CreateEventTemplate(dragEvent, draggedNode, currentColumn);
					if (!m_View->HandleWindowEvent(dragEvent) || !dragEvent.IsAllowed())
					{
						return;
					}

					if (wxDataObjectSimple* dragObject = dragEvent.GetDataObject())
					{
						DropSource dragSource(this, draggedRow);
						dragSource.SetData(*dragObject);

						m_DragSource = &dragSource;
						dragSource.DoDragDrop(dragEvent.GetDragFlags());
						m_DragSource = nullptr;
					}
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
		if (currentRow >= GetRowCount() || !currentNode || !currentColumn)
		{
			// Follow Windows convention here - clicking either left or right (but not middle) button clears the existing selection.
			if (m_View && (event.LeftDown() || event.RightDown()))
			{
				if (!m_SelectionStore.IsEmpty())
				{
					m_View->UnselectAll();
					SendSelectionChangedEvent(nullptr, currentColumn);
				}
			}

			event.Skip();
			return;
		}

		auto TestExpanderButton = [this, xpos, x, y](Row row, int itemOffset = -1, const Node* node = nullptr)
		{
			if (itemOffset < 0)
			{
				itemOffset = m_Indent * node->GetIndentLevel();
			}

			wxRect rect(xpos + itemOffset, GetRowStart(row) + (GetRowHeight(row) - m_UniformRowHeight) / 2, m_UniformRowHeight, m_UniformRowHeight);
			return rect.Contains(x, y);
		};

		// Test whether the mouse is hovering over the expander (a.k.a tree "+"
		// button) and also determine the offset of the real cell start, skipping
		// the indentation and the expander itself.
		Column* expander = m_View->GetExpanderColumnOrFirstOne();
		bool isHoverOverExpander = false;
		int itemOffset = 0;
		if (!IsList() && expander == currentColumn)
		{
			itemOffset = m_Indent * currentNode->GetIndentLevel();

			// We make the rectangle we are looking in a bit bigger than the actual
			// visual expander so the user can hit that little thing reliably.
			if (currentNode->HasChildren() && TestExpanderButton(currentRow, itemOffset))
			{
				// So the mouse is over the expander
				if (m_TreeNodeUnderMouse && m_TreeNodeUnderMouse != currentNode)
				{
					RefreshRow(GetRowByNode(*m_TreeNodeUnderMouse));
				}
				if (m_TreeNodeUnderMouse != currentNode)
				{
					RefreshRow(currentRow);
				}

				m_TreeNodeUnderMouse = currentNode;
				isHoverOverExpander = true;
			}

			// Account for the expander as well, even if this item doesn't have it,
			// its parent does so it still counts for the offset.
			itemOffset += m_UniformRowHeight;
		}

		if (!isHoverOverExpander && m_TreeNodeUnderMouse)
		{
			Row row = GetRowByNode(*m_TreeNodeUnderMouse);
			m_TreeNodeUnderMouse = nullptr;
			RefreshRow(row);
		}

		bool simulateClick = false;
		bool ignoreOtherColumns = expander != currentColumn && currentNode->HasChildren();
		if (event.ButtonDClick())
		{
			m_LastOnSame = false;
		}

		if (event.LeftDClick())
		{
			if (!isHoverOverExpander && (currentRow == m_RowLastClicked))
			{
				Event evt(EvtITEM_ACTIVATED);
				CreateEventTemplate(evt, currentNode, currentColumn);
				if (m_View->ProcessWindowEvent(evt))
				{
					// Item activation was handled from the user code.
					return;
				}
			}

			// Either it was a double click over the expander, or the second click
			// happened on another item than the first one or it was a bona fide
			// double click which was unhandled. In all these cases we continue
			// processing this event as a simple click, e.g. to select the item or
			// activate the renderer.
			simulateClick = true;
		}

		if (event.LeftUp() && !isHoverOverExpander)
		{
			if (m_RowSelectSingleOnUp)
			{
				// Select single line
				if (UnselectAllRows(m_RowSelectSingleOnUp))
				{
					SelectRow(m_RowSelectSingleOnUp, true);
					SendSelectionChangedEvent(GetNodeByRow(m_RowSelectSingleOnUp), currentColumn);
				}
				else if (m_View->IsStyleEnabled(CtrlStyle::CellFocus))
				{
					RefreshRow(currentRow);
				}
				// Else it was already selected, nothing to do.
			}

			m_LastOnSame = false;
			m_RowSelectSingleOnUp.MakeNull();
		}
		else if (!event.LeftUp())
		{
			// This is necessary, because after a DnD operation in
			// from and to ourself, the up event is swallowed by the
			// DnD code. So on next non-up event (which means here and
			// now) 'm_RowSelectSingleOnUp' should be reset.
			m_RowSelectSingleOnUp.MakeNull();
		}

		if (event.RightDown())
		{
			m_RowBeforeLastClicked = m_RowLastClicked;
			m_RowLastClicked = currentRow;

			// If the item is already selected, do not update the selection.
			// Multi-selections should not be cleared if a selected item is clicked.
			if (!IsRowSelected(currentRow))
			{
				UnselectAllRows();

				const Row oldCurrent = m_CurrentRow;
				ChangeCurrentRow(currentRow);
				SelectRow(m_CurrentRow, true);
				RefreshRow(oldCurrent);
				SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), currentColumn);
			}
		}
		//else if (event.MiddleDown())
		//{
		//}

	
		if ((event.LeftDown() || simulateClick) && isHoverOverExpander && !event.LeftDClick())
		{
			// hoverOverExpander being true tells us that our node must be
			// valid and have children. So we don't need any extra checks.
			if (currentNode->IsNodeExpanded())
			{
				Collapse(currentRow);
			}
			else
			{
				Expand(currentRow);
			}
		}
		else if ((event.LeftDown() || simulateClick) && !isHoverOverExpander)
		{
			m_RowBeforeLastClicked = m_RowLastClicked;
			m_RowLastClicked = currentRow;

			Row oldCurrentRow = m_CurrentRow;
			Column* const oldCurrentCol = m_CurrentColumn;
			bool oldWasSelected = IsRowSelected(m_CurrentRow);

			bool cmdModifierDown = event.CmdDown();
			if (IsSingleSelection() || !(cmdModifierDown || event.ShiftDown()))
			{
				if (IsSingleSelection() || !IsRowSelected(currentRow))
				{
					ChangeCurrentRow(currentRow);
					if (UnselectAllRows(currentRow))
					{
						SelectRow(m_CurrentRow, true);
						SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), currentColumn);
					}
				}
				else 
				{
					// Multi selection & current is highlighted & no mod keys
					m_RowSelectSingleOnUp = currentRow;
					ChangeCurrentRow(currentRow); // change focus
				}
			}
			else
			{
				// Multi selection & either ctrl or shift is down
				if (cmdModifierDown)
				{
					ChangeCurrentRow(currentRow);
					ReverseRowSelection(m_CurrentRow);
					SendSelectionChangedEvent(GetNodeByRow(m_CurrentRow), currentColumn);
				}
				else if (event.ShiftDown())
				{
					ChangeCurrentRow(currentRow);

					Row lineFrom = oldCurrentRow;
					Row lineTo = currentRow;

					if (!lineFrom)
					{
						// If we hadn't had any current row before, treat this as a simple click and select the new row only.
						lineFrom = currentRow;
					}

					if (lineTo < lineFrom)
					{
						lineTo = lineFrom;
						lineFrom = m_CurrentRow;
					}

					SelectRows(lineFrom, lineTo);

					wxSelectionStore::IterationState cookie;
					auto firstSel = m_SelectionStore.GetFirstSelectedItem(cookie);
					if (firstSel != wxSelectionStore::NO_SELECTION)
					{
						SendSelectionChangedEvent(GetNodeByRow(firstSel), currentColumn);
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
			if (oldCurrentCol != currentColumn && m_View->IsStyleEnabled(CtrlStyle::CellFocus))
			{
				RefreshRow(currentRow);
			}

			// Update selection here...
			m_CurrentColumn = currentColumn;
			m_IsCurrentColumnSetByKeyboard = false;

			// This flag is used to decide whether we should start editing the item
			// label. We do it if the user clicks twice (but not double clicks,
			// i.e. simulateClick is false) on the same item but not if the click
			// was used for something else already, e.g. selecting the item (so it
			// must have been already selected) or giving the focus to the control
			// (so it must have had focus already).
			m_LastOnSame = !simulateClick && ((currentColumn == oldCurrentCol) &&	(currentRow == oldCurrentRow)) && oldWasSelected && HasFocus();

			// Call OnActivateCell() after everything else as under GTK+
			if (IsCellInteractible(*currentNode, *currentColumn, InteractibleCell::Activator))
			{
				// Notify cell about click
				wxRect cellRect(xpos + itemOffset, GetRowStart(currentRow), currentColumn->GetWidth() - itemOffset, GetRowHeight(currentRow));

				// Note that SetupCellAttributes() should be called after GetRowStart()
				// call in 'cellRect' initialization above as GetRowStart() calls
				// SetupCellAttributes() for other items from inside it.
				Renderer& renderer = currentNode->GetRenderer(*currentColumn);
				renderer.BeginCellSetup(*currentNode, *currentColumn);
				renderer.SetupCellAttributes(GetCellStateForRow(oldCurrentRow));
				renderer.SetupCellValue();

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
				const wxAlignment align = renderer.GetEffectiveAlignment();

				wxRect rectItem = cellRect;
				const wxSize size = renderer.GetCellSize();
				if (size.x >= 0 && size.x < cellRect.width)
				{
					if (align & wxALIGN_CENTER_HORIZONTAL)
					{
						rectItem.x += (cellRect.width - size.x) / 2;
					}
					else if (align & wxALIGN_RIGHT)
					{
						rectItem.x += cellRect.width - size.x;
					}
					// else: wxALIGN_LEFT is the default
				}

				if (size.y >= 0 && size.y < cellRect.height)
				{
					if (align & wxALIGN_CENTER_VERTICAL)
					{
						rectItem.y += (cellRect.height - size.y)/2;
					}
					else if (align & wxALIGN_BOTTOM)
					{
						rectItem.y += cellRect.height - size.y;
					}
					// else: wxALIGN_TOP is the default
				}

				wxMouseEvent event2(event);
				event2.m_x -= rectItem.x;
				event2.m_y -= rectItem.y;
				m_View->CalcUnscrolledPosition(event2.m_x, event2.m_y, &event2.m_x, &event2.m_y);

				renderer.CallOnActivateCell(*currentNode, cellRect, &event2);
				renderer.EndCellSetup();
			}
		}
	}
	void MainWindow::OnSetFocus(wxFocusEvent& event)
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
	void MainWindow::OnKillFocus(wxFocusEvent& event)
	{
		m_HasFocus = false;

		if (HasCurrentRow())
		{
			Refresh();
		}
		event.Skip();
	}

	void MainWindow::OnTooltipEvent(wxTimerEvent& event)
	{
		RemoveTooltip();

		if (m_HotTrackColumn)
		{
			if (const Node* node = GetNodeByRow(m_HotTrackRow))
			{
				ShowToolTip(*node, *m_HotTrackColumn);
			}
		}
	}

	bool MainWindow::SendExpanderEvent(wxEventType type, Node& item)
	{
		Event event(type);
		CreateEventTemplate(event, &item);

		return !m_View->ProcessWindowEvent(event) || event.IsAllowed();
	}
	void MainWindow::SendSelectionChangedEvent(Node* item, Column* column)
	{
		if (item)
		{
			RefreshRow(GetRowByNode(*item));
		}

		Event event(EvtITEM_SELECTED);
		CreateEventTemplate(event, item, column);
		m_View->ProcessWindowEvent(event);
	}
	bool MainWindow::SendEditingStartedEvent(Node& item, Editor* editor)
	{
		Event event(EvtITEM_EDIT_STARTED);
		CreateEventTemplate(event, &item, editor->GetColumn());

		m_View->ProcessWindowEvent(event);
		return event.IsAllowed();
	}
	bool MainWindow::SendEditingDoneEvent(Node& item, Editor* editor, bool canceled, const wxAny& value)
	{
		EventEditor event(EvtITEM_EDIT_DONE);
		CreateEventTemplate(event, &item, editor->GetColumn());
		event.SetEditCanceled(canceled);
		event.SetValue(value);

		m_View->ProcessWindowEvent(event);
		return event.IsAllowed();
	}

	// Drawing
	void MainWindow::OnPaint(wxPaintEvent& event)
	{
		using namespace KxFramework;

		const wxSize clientSize = GetClientSize();
		wxAutoBufferedPaintDC paintDC(this);
		paintDC.SetPen(*wxTRANSPARENT_PEN);
		paintDC.SetBrush(m_View->GetBackgroundColour());
		paintDC.DrawRectangle(clientSize);

		m_View->PrepareDC(paintDC);
		wxGCDC dc(paintDC);
		wxGraphicsContext& gc = *dc.GetGraphicsContext();
		gc.SetAntialiasMode(wxANTIALIAS_NONE);
		gc.SetInterpolationQuality(wxINTERPOLATION_NONE);

		wxRendererNative& nativeRenderer = wxRendererNative::Get();

		if (m_BackgroundBitmap.IsOk())
		{
			wxPoint pos;
			if (m_BackgroundBitmapAlignment & wxALIGN_RIGHT)
			{
				pos.x = clientSize.x - m_BackgroundBitmap.GetWidth();
			}
			if (m_BackgroundBitmapAlignment & wxALIGN_BOTTOM)
			{
				pos.y = clientSize.y - m_BackgroundBitmap.GetHeight();
			}

			if (m_FitBackgroundBitmap && m_BackgroundBitmap.GetSize() != clientSize)
			{
				gc.DrawBitmap(m_BackgroundBitmap, pos.x, pos.y, clientSize.GetWidth(), clientSize.GetHeight());
			}
			else
			{
				dc.DrawBitmap(m_BackgroundBitmap, pos);
			}
		}

		const size_t columnCount = m_View->GetColumnCount();
		if (IsEmpty() || columnCount == 0)
		{
			if (!m_EmptyControlLabel.IsEmpty())
			{
				const int y = GetCharHeight() * 2;
				const wxRect rect(0, y, clientSize.GetWidth(), clientSize.GetHeight() - y);

				paintDC.SetTextForeground(m_View->GetForegroundColour().MakeDisabled());
				paintDC.DrawLabel(m_EmptyControlLabel, rect, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP);
			}

			// We assume that we have at least one column below and painting an empty control is unnecessary anyhow
			return;
		}

		wxRect updateRect = GetUpdateRegion().GetBox();
		m_View->CalcUnscrolledPosition(updateRect.x, updateRect.y, &updateRect.x, &updateRect.y);

		// Compute which rows needs to be redrawn
		const Row rowStart = GetRowAt(std::max(0, updateRect.y));
		const size_t rowCount = std::min(GetRowAt(std::max(0, updateRect.y + updateRect.height)) - rowStart + 1, GetRowCount() - rowStart);
		const Row rowEnd = rowStart + rowCount;

		// Send the event to the control itself.
		{
			Event cacheEvent(EvtVIEW_CACHE_HINT);
			CreateEventTemplate(cacheEvent);
			cacheEvent.SetCacheHints(rowStart, rowEnd - 1);
			m_View->ProcessWindowEvent(cacheEvent);
		}

		// Compute which columns needs to be redrawn
		// Calc start of X coordinate
		size_t coulumnIndexStart = 0;
		int xCoordStart = 0;
		for (coulumnIndexStart = 0; coulumnIndexStart < columnCount; coulumnIndexStart++)
		{
			const Column* column = m_View->GetColumnDisplayedAt(coulumnIndexStart);

			int width = 0;
			if (column->IsExposed(width))
			{
				if (xCoordStart + width >= updateRect.x)
				{
					break;
				}
				xCoordStart += width;
			}
		}

		
		// Calc end of X coordinate and visible columns count
		size_t visibleColumnsCount = 0;
		size_t coulmnIndexEnd = coulumnIndexStart;
		int xCoordEnd = xCoordStart;
		const int fullRowWidth = GetRowWidth();

		for (; coulmnIndexEnd < columnCount; coulmnIndexEnd++)
		{
			const Column* column = m_View->GetColumnDisplayedAt(coulmnIndexEnd);

			int width = 0;
			if (column->IsExposed(width))
			{
				visibleColumnsCount++;

				if (xCoordEnd > updateRect.GetRight())
				{
					// If we drawing only part of the control, draw it one pixel wider, to hide not drawn regions.
					if (xCoordEnd + width < fullRowWidth)
					{
						xCoordEnd++;
					}
					break;
				}
				xCoordEnd += width;
			}
		}

		// Draw background of alternate rows specially if required
		if (m_View->IsStyleEnabled(CtrlStyle::AlternatingRowColors))
		{
			KxColor altRowColor = m_View->m_AlternateRowColor;
			if (!altRowColor)
			{
				// Determine the alternate rows color automatically from the background color.
				const wxColour bgColor = m_View->GetBackgroundColour();

				// Depending on the background, alternate row color will be 3% more dark or 50% brighter.
				int alpha = bgColor.GetRGB() > 0x808080 ? 97 : 150;
				altRowColor = bgColor.ChangeLightness(alpha);
				
				if (m_BackgroundBitmap.IsOk())
				{
					altRowColor.SetAlpha8(200);
				}
			}

			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetBrush(altRowColor);

			// We only need to draw the visible part, so limit the rectangle to it.
			const int x = m_View->CalcUnscrolledPosition(wxPoint(0, 0)).x;
			const int widthRect = clientSize.x;
			for (size_t currentRow = rowStart; currentRow < rowEnd; currentRow++)
			{
				if (currentRow % 2)
				{
					dc.DrawRectangle(x, GetRowStart(currentRow), widthRect, GetRowHeight(currentRow));
				}
			}
		}

		const bool verticalRulesEnabled = m_View->IsStyleEnabled(CtrlStyle::VerticalRules);
		const bool horizontalRulesEnabled = m_View->IsStyleEnabled(CtrlStyle::HorizontalRules);

		// Redraw all cells for all rows which must be repainted and all columns
		const Column* const expanderColumn = m_View->GetExpanderColumnOrFirstOne();
		for (Row currentRow = rowStart; currentRow < rowEnd; ++currentRow)
		{
			const Node* const node = GetNodeByRow(currentRow);
			if (node == nullptr)
			{
				continue;
			}

			const wxRect cellInitialRect(xCoordStart, GetRowStart(currentRow), 0, GetRowHeight(currentRow));
			wxRect cellRect = cellInitialRect;
			const CellState cellState = GetCellStateForRow(currentRow);

			const bool isCategoryRow = node->IsCategoryNode();
			int categoryRowOffset = -1;

			int expanderIndent = 0;
			wxRect expanderRect;
			wxRect focusCellRect;

			auto GetRowRect = [&cellInitialRect, &expanderIndent, xCoordEnd, xCoordStart, expanderColumn]()
			{
				wxRect rowRect = cellInitialRect;
				rowRect.SetWidth(xCoordEnd - xCoordStart);
				if (expanderColumn->IsDisplayedFirst())
				{
					rowRect.x += expanderIndent;
					rowRect.width -= expanderIndent;
				}
				return rowRect;
			};

			for (size_t currentColumnIndex = coulumnIndexStart; currentColumnIndex < coulmnIndexEnd; currentColumnIndex++)
			{
				Column* column = m_View->GetColumnDisplayedAt(currentColumnIndex);
				if (!column->IsExposed(cellRect.width))
				{
					continue;
				}

				// Calculate expander button rect and its indent
				if (!IsList() && column == expanderColumn)
				{
					// Calculate the indent first
					const int indentOffset = m_Indent * node->GetIndentLevel();
					expanderIndent = std::min(indentOffset + m_UniformRowHeight, expanderColumn->GetWidth());

					if (node->HasChildren())
					{
						auto ClacExpanderRect = [&expanderRect, &cellRect, indentOffset](const wxSize& size, int margin = 0, int offset = 0)
						{
							// We reserve 'm_UniformRowHeight' of horizontal space for the expander but leave EXPANDER_MARGIN around the expander itself
							expanderRect.SetX(cellRect.x + indentOffset + margin);
							expanderRect.SetY(cellRect.y + (cellRect.height - size.GetHeight()) / 2 + margin - offset);
							expanderRect.SetWidth(size.GetWidth());
							expanderRect.SetHeight(size.GetHeight());
						};

						if (isCategoryRow)
						{
							ClacExpanderRect(nativeRenderer.GetCollapseButtonSize(this, paintDC), EXPANDER_MARGIN);
						}
						else
						{
							ClacExpanderRect(wxSize(m_UniformRowHeight, m_UniformRowHeight), EXPANDER_MARGIN, EXPANDER_OFFSET);
						}
					}
				}

				// Calc focus rect
				if (column == m_CurrentColumn && focusCellRect.IsEmpty())
				{
					focusCellRect = cellRect;

					const bool first = column->IsDisplayedFirst();
					if (column == expanderColumn && first)
					{
						focusCellRect.x += expanderIndent;
						focusCellRect.width -= expanderIndent;

						if (!verticalRulesEnabled)
						{
							focusCellRect.width += 1;
						}
					}
					if (!first)
					{
						focusCellRect.x -= 1;
						if (column->IsDisplayedLast())
						{
							focusCellRect.width += 1;
						}
						else
						{
							focusCellRect.width += verticalRulesEnabled ? 1 : 2;
						}
					}
				}

				// Move coordinates to next column
				cellRect.x += cellRect.width;
			}

			cellRect = cellInitialRect;
			for (size_t currentColumnIndex = coulumnIndexStart; currentColumnIndex < coulmnIndexEnd; currentColumnIndex++)
			{
				Column* column = m_View->GetColumnDisplayedAt(currentColumnIndex);
				if (!column->IsExposed(cellRect.width))
				{
					continue;
				}

				// Adjust cell rectangle
				wxRect adjustedCellRect = cellRect;
				adjustedCellRect.Deflate(PADDING_RIGHTLEFT, 0);

				if (column == expanderColumn)
				{
					adjustedCellRect.x += expanderIndent;
					adjustedCellRect.width -= expanderIndent;
				}

				// Draw vertical rules but don't draw the rule for last column is we have only one column
				if (verticalRulesEnabled && visibleColumnsCount > 1)
				{
					wxDCPenChanger pen(dc, m_PenRuleV);
					wxDCBrushChanger brush(dc, *wxTRANSPARENT_BRUSH);

					// Draw vertical rules in column's last pixel, so they will align with header control dividers
					const int x = cellRect.x + cellRect.width - 1;
					int yAdd = 0;
					if (currentRow + 1 == rowEnd)
					{
						yAdd = clientSize.GetHeight();
					}
					dc.DrawLine(x, cellRect.GetTop(), x, cellRect.GetBottom() + yAdd);
				}

				// Draw horizontal rules
				if (horizontalRulesEnabled)
				{
					wxDCPenChanger pen(dc, m_PenRuleV);
					wxDCBrushChanger brush(dc, *wxTRANSPARENT_BRUSH);

					dc.DrawLine(xCoordStart, cellInitialRect.GetY(), xCoordEnd + clientSize.GetWidth(), cellInitialRect.GetY());
				}

				// Clip DC to current column
				const wxRect columnRect(cellRect.GetX(), 0, cellRect.GetWidth(), m_virtualSize.GetHeight());
				DCClip clipDC(paintDC, columnRect);
				GCClip clipGC(gc, columnRect);

				// Draw the cell
				if (!isCategoryRow || currentColumnIndex == 0)
				{
					Renderer& renderer = node->GetRenderer(*column);
					renderer.BeginCellRendering(*node, *column, dc, &paintDC);

					renderer.SetupCellValue();
					renderer.SetupCellAttributes(cellState);
					renderer.CallDrawCellBackground(cellRect, cellState);

					// Draw cell content
					renderer.CallDrawCellContent(adjustedCellRect, GetCellStateForRow(currentRow));

					// Draw selection and hot-track indicator after background and cell content
					if (cellState.IsSelected() || cellState.IsHotTracked())
					{
						RenderEngine::DrawSelectionRect(this, paintDC, GetRowRect(), cellState.ToItemState(this));
					}

					#if 0
					// Measure category line offset
					if (isCategoryRow && categoryRowOffset < 0)
					{
						categoryRowOffset = renderer.GetCellSize().GetWidth() + GetCharWidth();
						if (column == expanderColumn)
						{
							categoryRowOffset += expanderIndent;
						}
					}
					#endif

					renderer.EndCellRendering();
				}

				// Move coordinates to next column
				cellRect.x += cellRect.width;

				// Draw expander
				if (column == expanderColumn && !expanderRect.IsEmpty())
				{
					int flags = 0;
					if (m_TreeNodeUnderMouse == node)
					{
						flags |= wxCONTROL_CURRENT;
					}
					if (node->IsNodeExpanded())
					{
						flags |= wxCONTROL_EXPANDED;
					}

					if (isCategoryRow)
					{
						wxRect rect(expanderRect.GetPosition(), nativeRenderer.GetCollapseButtonSize(this, dc));
						rect = rect.CenterIn(expanderRect);

						nativeRenderer.DrawCollapseButton(this, paintDC, rect, flags);
					}
					else if (m_View->IsExtraStyleEnabled(CtrlExtraStyle::PlusMinusExpander))
					{
						RenderEngine::DrawPlusMinusExpander(this, dc, expanderRect, flags);
					}
					else
					{
						if (UxTheme theme(*this, UxThemeClass::TreeView); theme)
						{
							const int partID = flags & wxCONTROL_CURRENT ? TVP_HOTGLYPH : TVP_GLYPH;
							const int stateID = flags & wxCONTROL_EXPANDED ? GLPS_OPENED : GLPS_CLOSED;

							wxRect rect(expanderRect.GetPosition(), theme.GetPartSize(dc, partID, stateID));
							theme.DrawBackground(paintDC, partID, stateID, rect.CenterIn(expanderRect));
						}
						else
						{
							nativeRenderer.DrawTreeItemButton(this, paintDC, expanderRect, flags);
						}
					}
				}

				// Draw cell focus
				if (m_HasFocus && m_View->IsStyleEnabled(CtrlStyle::CellFocus) && !focusCellRect.IsEmpty() && currentRow == m_CurrentRow && cellState.IsSelected())
				{
					// Focus rect looks ugly in it's narrower 3px
					if (focusCellRect.GetWidth() > 3)
					{
						nativeRenderer.DrawFocusRect(this, paintDC, wxRect(focusCellRect).Deflate(FromDIP(wxSize(1, 1))), wxCONTROL_SELECTED);
					}
				}

				// Draw drop hint
				#if wxUSE_DRAG_AND_DROP
				if (cellState.IsDropTarget())
				{
					wxRect rowRect = GetRowRect();
					nativeRenderer.DrawFocusRect(this, paintDC, rowRect, wxCONTROL_SELECTED);
				}
				#endif
			}

			// This needs more work
			#if 0
			if (isCategoryRow)
			{
				if (categoryRowOffset < 0)
				{
					categoryRowOffset = 0;
				}

				wxPoint pos1(cellInitialRect.GetX() + categoryRowOffset, cellRect.GetY() + cellRect.GetHeight() / 2);
				wxPoint pos2(cellInitialRect.GetX() + xCoordEnd - xCoordStart - categoryRowOffset, pos1.y);
				dc.DrawLine(pos1, pos2);
			}
			#endif
		}
	}
	CellState MainWindow::GetCellStateForRow(Row row) const
	{
		CellState state;
		if (row)
		{
			if (IsRowSelected(row))
			{
				state.SetSelected();
			}
			if (m_HotTrackRowEnabled && row == m_HotTrackRow && m_HotTrackColumn)
			{
				state.SetHotTracked();
			}
			if (m_DropHint && row == m_DropHintLine)
			{
				state.SetDropTarget();
			}
		}
		return state;
	}

	void MainWindow::UpdateDisplay()
	{
		m_Dirty = true;
	}
	void MainWindow::RefreshDisplay()
	{
		m_RedrawNeeded = true;
	}

	void MainWindow::RecalculateDisplay()
	{
		if (m_Model)
		{
			SetVirtualSize(GetRowWidth(), GetRowStart(GetRowCount()));
			m_View->SetScrollRate(std::min(GetCharWidth() * 2, m_UniformRowHeight), m_UniformRowHeight);
		}
		Refresh();
	}
	void MainWindow::DoSetVirtualSize(int x, int y)
	{
		if (x != m_virtualSize.x || y != m_virtualSize.y)
		{
			wxWindow::DoSetVirtualSize(x, y);
		}
	}

	// Tooltip
	bool MainWindow::ShowToolTip(const Node& node, Column& column)
	{
		// Get tooltip
		Renderer& renderer = node.GetRenderer(column);
		renderer.BeginCellSetup(node, column);
		renderer.SetupCellValue();
		ToolTip tooltip = node.GetToolTip(column);
		renderer.EndCellSetup();

		if (tooltip.IsOK())
		{
			// See if we need to display the tooltip at all
			bool shouldShow = true;
			if (tooltip.ShouldDisplayOnlyIfClipped())
			{
				// Setup renderer to get cell size
				const Column& clipTestColumn = tooltip.SelectClipTestColumn(column);

				Renderer& clipTestRenderer = node.GetRenderer(clipTestColumn);
				clipTestRenderer.BeginCellSetup(node, const_cast<Column&>(clipTestColumn));
				clipTestRenderer.SetupCellValue();
				const wxSize cellSize = clipTestRenderer.GetCellSize();
				clipTestRenderer.EndCellSetup();

				// Test for clipping
				shouldShow = false;
				const wxRect cellRect = clipTestColumn.GetRect();
				auto CompareWidth = [&cellSize](int width)
				{
					return width - (2 * PADDING_RIGHTLEFT) <= cellSize.GetWidth();
				};

				// If the column is too small to display its content
				shouldShow = shouldShow || CompareWidth(cellRect.GetWidth());

				// If the column scrolled outside of the visible area
				shouldShow = shouldShow || CompareWidth(GetClientSize().GetWidth() - cellRect.GetX());
			}

			// Show it
			if (shouldShow)
			{
				return tooltip.Show(node, column);
			}
		}
		return false;
	}
	void MainWindow::RemoveTooltip()
	{
		m_ToolTipTimer.Stop();
		m_ToolTip.Dismiss();
		SetToolTip(wxEmptyString);
	}

	// Columns
	void MainWindow::OnDeleteColumn(Column& column)
	{
		if (&column == m_HotTrackColumn)
		{
			m_HotTrackColumn = nullptr;
		}
		if (&column == m_CurrentColumn)
		{
			m_CurrentColumn = nullptr;
		}
	}
	void MainWindow::OnColumnCountChanged()
	{
		m_UseCellFocus = m_View->GetColumnCount() != 0;
		UpdateDisplay();
	}
	bool MainWindow::IsCellInteractible(const Node& node, const Column& column, InteractibleCell action) const
	{
		switch (action)
		{
			case InteractibleCell::Activator:
			{
				return node.IsActivatable(column);
			}
			case InteractibleCell::Editor:
			{
				return node.IsEditable(column);
			}
		};
		return false;
	}
	Column* MainWindow::FindInteractibleColumn(const Node& node, InteractibleCell action)
	{
		// Edit the current column editable in 'mode'. If no column is focused
		// (typically because the user has full row selected), try to find the
		// first editable column (this would typically be a checkbox for
		// wxDATAVIEW_CELL_ACTIVATABLE and we don't want to force the user to set
		// focus on the checkbox column; or on the only editable text column).

		Column* candidate = m_CurrentColumn;
		if (candidate && !IsCellInteractible(node, *candidate, action) && !m_IsCurrentColumnSetByKeyboard)
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
			const size_t columnCount = m_View->GetColumnCount();
			for (size_t columnIndex = 0; columnIndex < columnCount; columnIndex++)
			{
				Column* column = m_View->GetColumnDisplayedAt(columnIndex);
				if (column->IsVisible() && IsCellInteractible(node, *column, action))
				{
					candidate = column;
					break;
				}
			}
		}

		if (candidate && IsCellInteractible(node, *candidate, action))
		{
			return candidate;
		}
		return nullptr;
	}
	int MainWindow::CalcBestColumnWidth(Column& column) const
	{
		if (column.HasBestWidth())
		{
			return column.GetBestWidth();
		}

		const size_t rowCount = GetRowCount();
		MaxWidthCalculator calculator(const_cast<MainWindow*>(this), column, m_UniformRowHeight);
		calculator.UpdateWithWidth(column.GetMinWidth());

		if (m_View->HasHeaderCtrl())
		{
			calculator.UpdateWithWidth(m_View->GetHeaderCtrl()->GetColumnTitleWidth(column.GetNativeColumn()));
		}

		const wxPoint origin = m_View->CalcUnscrolledPosition(wxPoint(0, 0));
		calculator.ComputeBestColumnWidth(rowCount, GetRowAt(origin.y), GetRowAt(origin.y + GetClientSize().y));

		int maxWidth = calculator.GetMaxWidth();
		if (maxWidth > 0)
		{
			maxWidth += 2 * PADDING_RIGHTLEFT;
		}
		column.SetBestWidth(maxWidth);
		return maxWidth;
	}
	bool MainWindow::FitLastColumn(bool update)
	{
		if (!m_View->IsStyleEnabled(CtrlStyle::FitLastColumn))
		{
			return false;
		}

		size_t columnCount = m_View->GetVisibleColumnCount();
		if (columnCount != 0)
		{
			Column* lastVisibleColumn = m_View->GetColumnPhysicallyDisplayedAt(columnCount - 1);
			if (lastVisibleColumn)
			{
				auto SetColumnWidth = [lastVisibleColumn, update](int width)
				{
					if (update)
					{
						lastVisibleColumn->SetWidth(width);
					}
					else
					{
						lastVisibleColumn->AssignWidth(width);
					}
				};

				const int clientWidth = GetClientSize().GetWidth();
				const int virtualWidth = GetRowWidth();
				const int lastColumnLeft = virtualWidth - lastVisibleColumn->GetWidth();

				if (lastColumnLeft < clientWidth)
				{
					const bool fitToClient = m_View->IsExtraStyleEnabled(CtrlExtraStyle::FitLastColumnToClient);
					const int desiredWidth = std::max(clientWidth - lastColumnLeft, lastVisibleColumn->GetMinWidth());

					if (desiredWidth < lastVisibleColumn->CalcBestSize() && !fitToClient)
					{
						SetColumnWidth(lastVisibleColumn->GetWidth());
						SetVirtualSize(virtualWidth, m_virtualSize.y);
						return true;
					}
					SetColumnWidth(desiredWidth);

					// All columns fit on screen, so we don't need horizontal scrolling.
					// To prevent flickering scrollbar when resizing the window to be
					// narrower, force-set the virtual width to 0 here. It will eventually
					// be corrected at idle time.
					SetVirtualSize(0, m_virtualSize.y);
					RefreshRect(wxRect(lastColumnLeft, 0, clientWidth - lastColumnLeft, GetSize().y));

					return true;
				}
				else
				{
					// Don't bother, the columns won't fit anyway
					SetColumnWidth(lastVisibleColumn->GetWidth());
					SetVirtualSize(virtualWidth, m_virtualSize.y);

					return true;
				}
			}
		}
		return false;
	}

	// Items
	size_t MainWindow::RecalculateItemCount()
	{
		if (m_IsVirtualListModel)
		{
			return static_cast<VirtualListModel*>(m_Model)->GetItemCount();
		}
		else
		{
			return m_TreeRoot.GetSubTreeCount();
		}
	}
	void MainWindow::InvalidateItemCount()
	{
		m_ItemsCount = INVALID_COUNT;
		m_TreeNodeUnderMouse = nullptr;
		m_ToolTipTimer.Stop();
		CancelEdit();
	}
	void MainWindow::OnCellChanged(Node& node, Column* column)
	{
		// Move this node to its new correct place after it was updated.
		//
		// In principle, we could skip the call to PutInSortOrder() if the modified
		// column is not the sort column, but in real-world applications it's fully
		// possible and likely that custom compare uses not only the selected model
		// column but also falls back to other values for comparison. To ensure consistency
		// it is better to treat a value change as if it was an item change.

		node.PutInSortOrder();

		if (column == nullptr)
		{
			m_View->InvalidateColumnsBestWidth();
		}
		else
		{
			column->InvalidateBestWidth();
		}

		// Update the displayed value(s).
		RefreshRow(GetRowByNode(node));

		// Send event
		Event event(EvtITEM_VALUE_CHANGED);
		CreateEventTemplate(event, &node, column);
		m_View->ProcessWindowEvent(event);
	}
	void MainWindow::OnNodeAdded(Node& node)
	{
		InvalidateItemCount();
		m_IsRepresentingList = IsRepresentingList();

		if (IsVirtualList())
		{
			m_SelectionStore.OnItemsInserted(node.GetRow(), 1);
		}

		m_View->InvalidateColumnsBestWidth();
		UpdateDisplay();
	}
	void MainWindow::OnNodeRemoved(Node& item, intptr_t removedCount)
	{
		InvalidateItemCount();
		m_IsRepresentingList = IsRepresentingList();

		if (IsVirtualList())
		{
			m_SelectionStore.OnItemDelete(item.GetRow());
		}
		else
		{
			// Update selection by removing this node and its entire children tree from the selection.
			if (!m_SelectionStore.IsEmpty())
			{
				m_SelectionStore.OnItemsDeleted(item.GetRow(), removedCount);
			}
		}

		// Change the current row to the last row if the current exceed the max row number
		if (m_CurrentRow >= GetRowCount())
		{
			ChangeCurrentRow(m_ItemsCount - 1);
		}

		m_View->InvalidateColumnsBestWidth();

		m_TreeNodeUnderMouse = nullptr;
		UpdateDisplay();
	}
	void MainWindow::OnItemsCleared()
	{
		InvalidateItemCount();

		m_SelectionStore.Clear();
		m_CurrentRow.MakeNull();
		m_HotTrackRow.MakeNull();
		m_CurrentColumn = nullptr;
		m_HotTrackColumn = nullptr;
		m_TreeNodeUnderMouse = nullptr;
		m_IsRepresentingList = false;
		BuildTree();

		m_View->InvalidateColumnsBestWidth();
		UpdateDisplay();
	}
	void MainWindow::OnShouldResort()
	{
		if (!IsVirtualList())
		{
			m_TreeRoot.Resort(true);
		}
		UpdateDisplay();
	}

	void MainWindow::BuildTree()
	{
		DestroyTree();
		InvalidateItemCount();
	}
	void MainWindow::DestroyTree()
	{
		m_TreeRoot.ResetAll();
		if (!IsVirtualList())
		{
			m_ItemsCount = 0;
		}
	}
	void MainWindow::DoAssignModel(Model* model, bool own)
	{
		// Forget and (optionally) delete old model
		if (m_Model)
		{
			m_Model->OnDetachModel();
			m_Model->SetMainWindow(nullptr);
		}
		if (m_OwnModel)
		{
			delete m_Model;
		}

		// Assign new model and set main window
		m_Model = model;
		m_Model->SetMainWindow(this);

		m_OwnModel = own;
		m_IsVirtualListModel = model && model->QueryInterface<VirtualListModel>() != nullptr;

		// Call model event
		m_Model->OnAttachModel();
		ItemsChanged();
	}
	bool MainWindow::IsRepresentingList() const
	{
		if (m_IsVirtualListModel)
		{
			return true;
		}
		else
		{
			for (const Node* node: m_TreeRoot.GetChildren())
			{
				if (node->HasChildren())
				{
					return false;
				}
			}
			return true;
		}
	}

	// Misc
	void MainWindow::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();

		if (m_HotTrackRow && !IsMouseInWindow())
		{
			m_HotTrackRowEnabled = false;
			RefreshRow(m_HotTrackRow);

			m_HotTrackRow.MakeNull();
			m_HotTrackColumn = nullptr;

			RemoveTooltip();
		}

		if (m_Dirty)
		{
			RecalculateDisplay();
			FitLastColumn();

			m_Dirty = false;
			m_RedrawNeeded = false;
		}
		if (m_RedrawNeeded)
		{
			FitLastColumn(false);
			if (HeaderCtrl* header = m_View->GetHeaderCtrl())
			{
				header->DoUpdate();
			}
			RecalculateDisplay();

			m_RedrawNeeded = false;
		}
	}

	MainWindow::MainWindow(View* parent, wxWindowID id)
		:wxWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxBORDER_NONE, GetClassInfo()->GetClassName()),
		m_TreeRoot(this), m_VirtualNode(m_TreeRoot), m_View(parent)
	{
		// Setup drawing
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));

		KxColor rulesColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
		rulesColor.SetAlpha8(85);

		m_PenRuleH = rulesColor;
		m_PenRuleV = rulesColor;
		m_PenExpander = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
		m_UniformRowHeight = GetDefaultRowHeight();
		m_Indent = wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y);

		// Tooltip
		m_ToolTip.Create(this);
		m_ToolTipTimer.SetOwner(this, wxID_ANY);
		Bind(wxEVT_TIMER, &MainWindow::OnTooltipEvent, this);

		// Bind events
		Bind(wxEVT_PAINT, &MainWindow::OnPaint, this);
		Bind(wxEVT_SET_FOCUS, &MainWindow::OnSetFocus, this);
		Bind(wxEVT_KILL_FOCUS, &MainWindow::OnKillFocus, this);
		Bind(wxEVT_CHAR_HOOK, &MainWindow::OnCharHook, this);
		Bind(wxEVT_CHAR, &MainWindow::OnChar, this);

		Bind(wxEVT_LEFT_DOWN, &MainWindow::OnMouse, this);
		Bind(wxEVT_LEFT_UP, &MainWindow::OnMouse, this);
		Bind(wxEVT_LEFT_DCLICK, &MainWindow::OnMouse, this);

		//Bind(wxEVT_MIDDLE_DOWN, &KxDataViewMainWindow::OnMouse, this);
		//Bind(wxEVT_MIDDLE_UP, &KxDataViewMainWindow::OnMouse, this);
		//Bind(wxEVT_MIDDLE_DCLICK, &KxDataViewMainWindow::OnMouse, this);

		Bind(wxEVT_RIGHT_DOWN, &MainWindow::OnMouse, this);
		Bind(wxEVT_RIGHT_UP, &MainWindow::OnMouse, this);
		Bind(wxEVT_RIGHT_DCLICK, &MainWindow::OnMouse, this);

		Bind(wxEVT_MOTION, &MainWindow::OnMouse, this);
		Bind(wxEVT_ENTER_WINDOW, &MainWindow::OnMouse, this);
		Bind(wxEVT_LEAVE_WINDOW, &MainWindow::OnMouse, this);
		Bind(wxEVT_MOUSEWHEEL, &MainWindow::OnMouse, this);
		//Bind(wxEVT_CHILD_FOCUS, &MainWindow::OnMouse, this);
		
		//Bind(wxEVT_AUX1_DOWN, &MainWindow::OnMouse, this);
		//Bind(wxEVT_AUX1_UP, &MainWindow::OnMouse, this);
		//Bind(wxEVT_AUX1_DCLICK, &MainWindow::OnMouse, this);
		//Bind(wxEVT_AUX2_DOWN, &MainWindow::OnMouse, this);
		//Bind(wxEVT_AUX2_UP, &MainWindow::OnMouse, this);
		//Bind(wxEVT_AUX2_DCLICK, &MainWindow::OnMouse, this);
		//Bind(wxEVT_MAGNIFY, &MainWindow::OnMouse, this);

		// Do update
		UpdateDisplay();
	}
	MainWindow::~MainWindow()
	{
		m_TreeRoot.ResetAll();
		RemoveTooltip();

		if (m_OwnModel)
		{
			delete m_Model;
		}
		else if (m_Model)
		{
			m_Model->SetMainWindow(nullptr);
		}
	}

	void MainWindow::CreateEventTemplate(Event& event, Node* node, Column* column)
	{
		event.SetId(m_View->GetId());
		event.SetEventObject(m_View);
		event.SetNode(node);
		event.SetColumn(column);
	}
	void MainWindow::ItemsChanged()
	{
		m_ItemsCount = RecalculateItemCount();
		m_View->InvalidateColumnsBestWidth();
		Refresh();
	}

	// Refreshing
	void MainWindow::RefreshRows(Row from, Row to)
	{
		wxRect rect = GetRowsRect(from, to);
		m_View->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);

		wxSize clientSize = GetClientSize();
		wxRect clientRect(0, 0, clientSize.x, clientSize.y);
		wxRect intersectRect = clientRect.Intersect(rect);
		if (!intersectRect.IsEmpty())
		{
			RefreshRect(intersectRect, true);
		}
	}
	void MainWindow::RefreshRowsAfter(Row firstRow)
	{
		wxSize clientSize = GetClientSize();
		int start = GetRowStart(firstRow);
		m_View->CalcScrolledPosition(start, 0, &start, nullptr);

		if (start <= clientSize.y)
		{
			wxRect rect(0, start, clientSize.x, clientSize.y - start);
			RefreshRect(rect, true);
		}
	}
	void MainWindow::RefreshColumn(const Column& column)
	{
		wxSize size = GetClientSize();

		// Find X coordinate of this column
		int left = 0;
		for (size_t i = 0; i < m_View->GetColumnCount(); i++)
		{
			Column* currentColumn = m_View->GetColumnDisplayedAt(i);
			if (currentColumn)
			{
				if (currentColumn == &column)
				{
					break;
				}
				left += currentColumn->GetWidth();
			}
		}

		RefreshRect(wxRect(left, 0, column.GetWidth(), size.GetHeight()));
	}

	// Item rect
	wxRect MainWindow::GetRowsRect(Row rowFrom, Row rowTo) const
	{
		if (rowFrom > rowTo)
		{
			std::swap(rowFrom, rowTo);
		}

		wxRect rect;
		rect.x = 0;
		rect.y = GetRowStart(rowFrom);

		// Don't calculate exact width of the row, because GetEndOfLastCol() is
		// expensive to call, and controls with rows not spanning entire width rare.
		// It is more efficient to e.g. repaint empty parts of the window needlessly.
		rect.width = std::numeric_limits<decltype(rect.width)>::max();
		if (rowFrom == rowTo)
		{
			rect.height = GetRowHeight(rowFrom);
		}
		else
		{
			rect.height = GetRowStart(rowTo) - rect.y + GetRowHeight(rowTo);
		}
		return rect;
	}
	wxRect MainWindow::GetRowRect(Row row) const
	{
		return GetRowsRect(row, row);
	}
	int MainWindow::GetRowStart(Row row) const
	{
		if (m_View->IsStyleEnabled(CtrlStyle::VariableRowHeight))
		{
			size_t columnCount = m_View->GetColumnCount();
			int start = 0;

			for (size_t currentRow = 0; currentRow < row; currentRow++)
			{
				const Node* node = GetNodeByRow(currentRow);
				if (node)
				{
					int height = m_UniformRowHeight;

					for (size_t currentColumn = 0; currentColumn < columnCount; currentColumn++)
					{
						const Column* column = m_View->GetColumn(currentColumn);
						if (column->IsVisible())
						{
							height = GetVariableRowHeight(*node);
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
		return row * m_UniformRowHeight;
	}
	int MainWindow::GetRowHeight(Row row) const
	{
		if (m_View->IsStyleEnabled(CtrlStyle::VariableRowHeight))
		{
			const Node* node = GetNodeByRow(row);
			if (node)
			{
				size_t columnCount = m_View->GetColumnCount();

				int height = m_UniformRowHeight;
				for (size_t currentColumn = 0; currentColumn < columnCount; currentColumn++)
				{
					const Column* column = m_View->GetColumn(currentColumn);
					if (column->IsVisible())
					{
						height = GetVariableRowHeight(*node);
					}
				}
				return height;
			}
		}
		return m_UniformRowHeight;
	}
	int MainWindow::GetVariableRowHeight(const Node& node) const
	{
		int height = node.GetRowHeight();
		return height > 0 ? height : m_UniformRowHeight;
	}
	int MainWindow::GetVariableRowHeight(Row row) const
	{
		const Node* node = GetNodeByRow(row);
		return node ? GetVariableRowHeight(*node) : m_UniformRowHeight;
	}
	int MainWindow::GetRowWidth() const
	{
		int width = 0;
		for (size_t i = 0; i < m_View->GetColumnCount(); i++)
		{
			int widthThis = 0;
			if (m_View->GetColumn(i)->IsExposed(widthThis))
			{
				width += widthThis;
			}
		}
		return width;
	}
	Row MainWindow::GetRowAt(int yCoord) const
	{
		if (m_View->IsStyleEnabled(CtrlStyle::VariableRowHeight))
		{
			Row row = 0;
			int yPos = 0;
			while (true)
			{
				const Node* node = GetNodeByRow(row);
				if (node == nullptr)
				{
					// Not really correct
					return row + ((yCoord - yPos) / m_UniformRowHeight);
				}

				yPos += GetVariableRowHeight(*node);
				if (yCoord < yPos)
				{
					return row;
				}
				++row;
			}
		}
		return yCoord / m_UniformRowHeight;
	}

	void MainWindow::SetUniformRowHeight(int height)
	{
		m_UniformRowHeight = height > 0 ? height : GetDefaultRowHeight();
	}
	int MainWindow::GetDefaultRowHeight(UniformHeight type) const
	{
		int resultHeight = 0;
		const int iconMargin = 6 * KxFramework::System::GetMetric(wxSYS_BORDER_Y, this);
		const int iconHeight = KxFramework::System::GetMetric(wxSYS_SMALLICON_Y, this);

		switch (type)
		{
			case UniformHeight::Default:
			{
				int userHeight = wxSystemOptions::GetOptionInt("KxDataView2::DefaultRowHeight");
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
			case UniformHeight::ListView:
			{
				resultHeight = std::max(m_View->FromDIP(17), GetCharHeight() + iconMargin);
				break;
			}
			case UniformHeight::Explorer:
			{
				resultHeight = std::max(m_View->FromDIP(22), GetCharHeight() + iconMargin);
				break;
			}
		};
		return resultHeight >= iconHeight ? resultHeight : iconHeight;
	}

	// Drag and Drop
	wxBitmap MainWindow::CreateItemBitmap(Row row, int& indent)
	{
		int width = GetRowWidth();
		int height = GetRowHeight(row);

		indent = 0;
		if (!IsList())
		{
			Node* node = GetNodeByRow(row);
			indent = m_Indent * node->GetIndentLevel();
			indent = indent + m_UniformRowHeight;
		}
		width -= indent;
		wxRect itemRect = wxRect(0, 0, width, height);

		wxBitmap bitmap(width, height, 24);
		{
			wxMemoryDC memoryDC(bitmap);
			memoryDC.SetFont(GetFont());
			memoryDC.SetBackground(m_View->GetBackgroundColour());
			memoryDC.SetTextForeground(m_View->GetForegroundColour());
			memoryDC.SetTextBackground(m_View->GetBackgroundColour());
			memoryDC.Clear();

			// Draw selection
			RenderEngine::DrawSelectionRect(this, memoryDC, itemRect, wxCONTROL_CURRENT|wxCONTROL_SELECTED|wxCONTROL_FOCUSED);

			// Draw cells
			wxGCDC gcdc(memoryDC);

			int x = 0;
			Column* expander = m_View->GetExpanderColumnOrFirstOne();
			for (size_t columnIndex = 0; columnIndex < m_View->GetColumnCount(); columnIndex++)
			{
				Column* column = m_View->GetColumnDisplayedAt(columnIndex);
				if (column->IsExposed(width))
				{
					if (column == expander)
					{
						width -= indent;
					}

					CellState cellState = GetCellStateForRow(row);
					Node* node = GetNodeByRow(row);
					Renderer& renderer = node->GetRenderer(*column);
					renderer.BeginCellRendering(*node, *column, gcdc, &memoryDC);

					wxRect cellRect(x, 0, width, height);

					renderer.SetupCellValue();
					renderer.SetupCellAttributes(cellState);
					renderer.CallDrawCellBackground(cellRect, cellState);

					cellRect.Deflate(PADDING_RIGHTLEFT, 0);
					renderer.CallDrawCellContent(cellRect, cellState);

					renderer.EndCellRendering();
					x += width;
				}
			}
		}

		return bitmap;
	}

	bool MainWindow::EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop)
	{
		using Result = DnDInfo::Result;

		if (dataObject)
		{
			const auto result = m_DragDropInfo.EnableOperation(*dataObject, type, isPreferredDrop);
			if (result != Result::None && result != Result::OperationRemoved)
			{
				if (m_DragDropDataObject == nullptr)
				{
					m_DragDropDataObject = new wxDataObjectComposite();
					m_DropTarget = new DropTarget(m_DragDropDataObject, this);
					SetDropTarget(m_DropTarget);
				}

				if (m_DragDropDataObject->GetObject(dataObject->GetFormat()) == nullptr)
				{
					m_DragDropDataObject->Add(dataObject.release(), isPreferredDrop);
				}
			}
			return result != DnDInfo::Result::None;
		}
		return false;
	}
	bool MainWindow::DisableDND(const wxDataFormat& format)
	{
		using Result = DnDInfo::Result;

		const Result value = m_DragDropInfo.DisableOperations(format);
		return value != Result::OperationRemoved || value == Result::None;
	}

	std::tuple<Row, Node*> MainWindow::DragDropHitTest(const wxPoint& pos) const
	{
		// Get row
		wxPoint unscrolledPos;
		m_View->CalcUnscrolledPosition(pos.x, pos.y, &unscrolledPos.x, &unscrolledPos.y);
		Row row = GetRowAt(unscrolledPos.y);

		// Get item
		Node* node = nullptr;
		if (row < GetRowCount() && pos.y <= GetRowWidth())
		{
			node = GetNodeByRow(row);
		}

		return {row, node};
	}
	void MainWindow::RemoveDropHint()
	{
		if (m_DropHint)
		{
			m_DropHint = false;
			RefreshRow(m_DropHintLine);
			m_DropHintLine.MakeNull();
		}
	}
	wxDragResult MainWindow::OnDragOver(const wxDataObjectSimple& dataObject, const wxPoint& pos, wxDragResult dragResult)
	{
		auto [row, node] = DragDropHitTest(pos);
		if (row != 0 && row < GetRowCount())
		{
			const Row firstVisible = GetFirstVisibleRow();
			const Row lastVisible = GetLastVisibleRow();
			if (row == firstVisible || row == firstVisible + 1)
			{
				ScrollTo(row - 1);
			}
			else if (row == lastVisible || row == lastVisible - 1)
			{
				ScrollTo(firstVisible + 1);
			}
		}

		EventDND event(EvtITEM_DROP_POSSIBLE);
		CreateEventTemplate(event, node);
		event.SetPosition(pos);
		event.SetDropEffect(dragResult);
		event.SetDataObject(const_cast<wxDataObjectSimple*>(&dataObject));

		if (!m_View->HandleWindowEvent(event) || !event.IsAllowed())
		{
			RemoveDropHint();
			return wxDragNone;
		}

		if (node)
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
	wxDragResult MainWindow::OnDropData(wxDataObjectSimple& dataObject, const wxPoint& pos, wxDragResult dragResult)
	{
		auto [row, node] = DragDropHitTest(pos);

		EventDND event(EvtITEM_DROP);
		CreateEventTemplate(event, node);
		event.SetPosition(pos);
		event.SetDropEffect(dragResult);
		event.SetDataObject(&dataObject);

		if (!m_View->HandleWindowEvent(event) || !event.IsAllowed())
		{
			return wxDragNone;
		}
		return dragResult;
	}
	bool MainWindow::TestDropPossible(const wxDataObjectSimple& dataObject, const wxPoint& pos)
	{
		RemoveDropHint();
		auto [row, node] = DragDropHitTest(pos);

		EventDND event(EvtITEM_DROP_POSSIBLE);
		CreateEventTemplate(event, node);
		event.SetPosition(pos);
		event.SetDataObject(const_cast<wxDataObjectSimple*>(&dataObject));

		if (!m_View->HandleWindowEvent(event) || !event.IsAllowed())
		{
			return false;
		}
		return true;
	}
	
	wxDragResult MainWindow::OnDragDropEnter(const wxDataObjectSimple& dataObject, const wxPoint& pos, wxDragResult dragResult)
	{
		return dragResult;
	}
	void MainWindow::OnDragDropLeave()
	{
		RemoveDropHint();
	}

	// Scrolling
	void MainWindow::ScrollWindow(int dx, int dy, const wxRect* rect)
	{
		wxWindow::ScrollWindow(dx, dy, rect);
		if (wxHeaderCtrl* header = m_View->GetHeaderCtrl())
		{
			header->ScrollWindow(dx, 0);
		}
	}
	void MainWindow::ScrollTo(Row row, size_t column)
	{
		wxPoint pos;
		m_View->GetScrollPixelsPerUnit(&pos.x, &pos.y);
		wxPoint scrollPos(-1, GetRowStart(row) / pos.y);

		if (column != INVALID_COLUMN)
		{
			wxRect rect = GetClientRect();

			wxPoint unscrolledPos;
			m_View->CalcUnscrolledPosition(rect.x, rect.y, &unscrolledPos.x, &unscrolledPos.y);

			int columnWidth = 0;
			int x_start = 0;
			for (size_t colnum = 0; colnum < column; colnum++)
			{
				if (m_View->GetColumnDisplayedAt(colnum)->IsExposed(columnWidth))
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
		m_View->Scroll(scrollPos);
	}
	void MainWindow::EnsureVisible(Row row, size_t column)
	{
		if (row > GetRowCount())
		{
			row = GetRowCount();
		}

		size_t first = GetFirstVisibleRow();
		size_t last = GetLastVisibleRow();
		if (row < first)
		{
			ScrollTo(row, column);
		}
		else if (row > last)
		{
			ScrollTo((intptr_t)row - (intptr_t)last + (intptr_t)first, column);
		}
		else
		{
			ScrollTo(first, column);
		}
	}

	// Current row and column
	void MainWindow::ChangeCurrentRow(Row row)
	{
		m_CurrentRow = row;
		// Send event ?
	}
	bool MainWindow::TryAdvanceCurrentColumn(Node* node, wxKeyEvent& event, bool moveForward)
	{
		const size_t columnCount = m_View->GetColumnCount();
		const size_t visibleColumnsCount = m_View->GetVisibleColumnCount();
		const bool wrapAround = event.GetKeyCode() == WXK_TAB;
		const bool currentColumnIsExpander = m_CurrentColumn && m_CurrentColumn->IsExpander();

		// Nothing to do
		if (columnCount == 0 || !m_UseCellFocus)
		{
			return false;
		}

		if (node && currentColumnIsExpander)
		{
			if (moveForward)
			{
				if (node->HasChildren() && !node->IsExpanded())
				{
					return false;
				}
			}
			else
			{
				if (node->HasParent())
				{
					return false;
				}
			}
		}

		if (m_CurrentColumn == nullptr || !m_IsCurrentColumnSetByKeyboard)
		{
			if (moveForward)
			{
				m_CurrentColumn = m_View->GetColumnDisplayedAt(0);
				m_IsCurrentColumnSetByKeyboard = true;
				RefreshRow(m_CurrentRow);
				return true;
			}
		}
		if (m_CurrentColumn == nullptr)
		{
			return false;
		}

		size_t nextColumn = std::clamp<intptr_t>((intptr_t)m_CurrentColumn->GetDisplayIndex() + (moveForward ? +1 : -1), 0, visibleColumnsCount);
		if (nextColumn == (intptr_t)visibleColumnsCount)
		{
			if (!wrapAround)
			{
				return false;
			}

			if (GetCurrentRow() < GetRowCount() - 1)
			{
				// Go to the first column of the next row:
				nextColumn = 0;
				OnVerticalNavigation(wxKeyEvent(), +1);
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
			if (GetCurrentRow().IsOK())
			{
				// Go to the last column of the previous row
				nextColumn = visibleColumnsCount - 1;
				OnVerticalNavigation(wxKeyEvent(), -1);
			}
			else
			{
				// Allow focus change
				event.Skip();
				return false;
			}
		}

		m_IsCurrentColumnSetByKeyboard = true;
		m_CurrentColumn = m_View->GetColumnDisplayedAt(nextColumn);
		
		EnsureVisible(m_CurrentRow, m_CurrentColumn->GetIndex());
		RefreshRow(m_CurrentRow);
		return true;
	}

	Node* MainWindow::GetHotTrackItem() const
	{
		if (m_HotTrackRow)
		{
			return GetNodeByRow(m_HotTrackRow);
		}
		return nullptr;
	}
	Column* MainWindow::GetHotTrackColumn() const
	{
		return m_HotTrackColumn;
	}

	// Selection
	bool MainWindow::UnselectAllRows(Row exceptThisRow)
	{
		if (!IsSelectionEmpty())
		{
			for (Row i = GetFirstVisibleRow(); i <= GetLastVisibleRow(); ++i)
			{
				if (m_SelectionStore.IsSelected(i) && i != exceptThisRow)
				{
					RefreshRow(i);
				}
			}

			if (exceptThisRow)
			{
				const bool wasSelected = m_SelectionStore.IsSelected(exceptThisRow);
				ClearSelection();
				if (wasSelected)
				{
					m_SelectionStore.SelectItem(exceptThisRow);

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
	void MainWindow::ReverseRowSelection(Row row)
	{
		m_SelectionStore.SelectItem(row, !IsRowSelected(row));
		RefreshRow(row);
	}
	void MainWindow::SelectRow(Row row, bool select)
	{
		if (m_SelectionStore.SelectItem(row, select))
		{
			RefreshRow(row);
		}
	}
	void MainWindow::SelectRows(Row from, Row to)
	{
		if (from > to)
		{
			std::swap(from, to);
		}
		for (Row row = from; row <= to; ++row)
		{
			m_SelectionStore.SelectItem(row, true);
		}
		RefreshRows(from, to);
	}
	void MainWindow::SelectRows(const Row::Vector& selection)
	{
		for (const Row& row: selection)
		{
			if (m_SelectionStore.SelectItem(row))
			{
				RefreshRow(row);
			}
		}
	}

	// View
	size_t MainWindow::GetRowCount() const
	{
		if (m_ItemsCount == INVALID_COUNT)
		{
			MainWindow* self = const_cast<MainWindow*>(this);
			self->UpdateItemCount(const_cast<MainWindow*>(this)->RecalculateItemCount());
			self->UpdateDisplay();
		}
		return m_ItemsCount;
	}
	size_t MainWindow::GetCountPerPage() const
	{
		wxSize size = GetClientSize();
		return size.y / m_UniformRowHeight;
	}
	Row MainWindow::GetFirstVisibleRow() const
	{
		wxPoint pos(0, 0);
		m_View->CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);
		return GetRowAt(pos.y);
	}
	Row MainWindow::GetLastVisibleRow() const
	{
		wxSize size = GetClientSize();
		m_View->CalcUnscrolledPosition(size.x, size.y, &size.x, &size.y);

		// We should deal with the pixel here.
		size_t row = GetRowAt(size.y) - 1;
		return std::min(GetRowCount() - 1, row);
	}

	void MainWindow::HitTest(const wxPoint& pos, Node** nodeOut, Column** columnOut)
	{
		wxPoint unscrolledPos = wxDefaultPosition;
		if (nodeOut)
		{
			m_View->CalcUnscrolledPosition(pos.x, pos.y, &unscrolledPos.x, &unscrolledPos.y);
			*nodeOut = GetNodeByRow(GetRowAt(unscrolledPos.y));
		}

		if (columnOut)
		{
			int x_start = 0;
			const size_t columnCount = m_View->GetColumnCount();

			if (!unscrolledPos.IsFullySpecified())
			{
				m_View->CalcUnscrolledPosition(pos.x, pos.y, &unscrolledPos.x, &unscrolledPos.y);
			}
			for (size_t colnumIndex = 0; colnumIndex < columnCount; colnumIndex++)
			{
				Column* column = m_View->GetColumnDisplayedAt(colnumIndex);

				int width = 0;
				if (column->IsExposed(width))
				{
					if (x_start + width >= unscrolledPos.x)
					{
						*columnOut = column;
						return;
					}
					x_start += width;
				}
			}
		}
	}
	wxRect MainWindow::GetItemRect(const Node& item, const Column* column)
	{
		int xpos = 0;
		int width = 0;
		size_t columnCount = m_View->GetColumnCount();

		// If column is null the loop will compute the combined width of all columns.
		// Otherwise, it will compute the x position of the column we are looking for.
		for (size_t i = 0; i < columnCount; i++)
		{
			Column* currentColumn = m_View->GetColumnDisplayedAt(i);
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

		Row row = GetRowByNode(item);
		if (!row)
		{
			// This means the row is currently not visible at all.
			return wxRect();
		}

		// We have to take an expander column into account and compute its indentation
		// to get the correct x position where the actual text is.
		int indent = 0;
		if (!IsList() && (!column || m_View->GetExpanderColumnOrFirstOne() == column))
		{
			Node* node = GetNodeByRow(row);
			indent = m_Indent * node->GetIndentLevel();
		
			// Use 'm_UniformLineHeight' as the width of the expander
			indent += m_UniformRowHeight;
		}

		wxRect itemRect(xpos + indent, GetRowStart(row), width - indent, GetRowHeight(row));
		m_View->CalcScrolledPosition(itemRect.x, itemRect.y, &itemRect.x, &itemRect.y);
		return itemRect;
	}
	
	// Rows
	void MainWindow::Expand(Row row)
	{
		if (!IsList())
		{
			if (Node* node = GetNodeByRow(row))
			{
				Expand(*node, row);
			}
		}
	}
	void MainWindow::Expand(Node& node, Row row)
	{
		if (node.HasChildren() && !node.IsNodeExpanded())
		{
			if (!SendExpanderEvent(EvtITEM_EXPANDING, node))
			{
				// Vetoed by the event handler.
				return;
			}

			const intptr_t rowsAdded = node.ToggleNodeExpanded();
			if (!row)
			{
				row = GetRowByNode(node);
			}

			// Shift all stored indices after this row by the number of newly added rows.
			m_SelectionStore.OnItemsInserted(row + 1, rowsAdded);
			if (m_CurrentRow > row)
			{
				ChangeCurrentRow(m_CurrentRow + rowsAdded);
			}

			if (m_ItemsCount != INVALID_COUNT)
			{
				m_ItemsCount += rowsAdded;
			}

			// Expanding this item means the previously cached column widths could
			// have become invalid as new items are now visible.
			m_View->InvalidateColumnsBestWidth();
			UpdateDisplay();

			// Send the expanded event
			SendExpanderEvent(EvtITEM_EXPANDED, node);
		}
	}

	void MainWindow::Collapse(Row row)
	{
		if (!IsList())
		{
			if (Node* node = GetNodeByRow(row))
			{
				Collapse(*node, row);
			}
		}
	}
	void MainWindow::Collapse(Node& node, Row row)
	{
		if (node.HasChildren() && node.IsNodeExpanded())
		{
			if (!SendExpanderEvent(EvtITEM_COLLAPSING, node))
			{
				// Vetoed by the event handler.
				return;
			}

			if (!row)
			{
				row = GetRowByNode(node);
			}

			const intptr_t rowsRemoved = node.GetSubTreeCount();
			if (m_SelectionStore.OnItemsDeleted(row + 1, rowsRemoved))
			{
				RefreshRow(row);

				if (!m_CurrentColumn)
				{
					m_CurrentColumn = m_View->GetExpanderColumnOrFirstOne();
				}
				SendSelectionChangedEvent(GetNodeByRow(row), m_CurrentColumn);
			}
			node.ToggleNodeExpanded();

			// Adjust the current row if necessary.
			if (m_CurrentRow > row)
			{
				// If the current row was among the collapsed items, make the
				// parent itself current.
				if (m_CurrentRow <= row + rowsRemoved)
				{
					ChangeCurrentRow(row);
				}
				else
				{
					// Otherwise just update the index.
					ChangeCurrentRow(m_CurrentRow - rowsRemoved);
				}
			}

			if (m_ItemsCount != INVALID_COUNT)
			{
				m_ItemsCount -= rowsRemoved;
			}

			m_View->InvalidateColumnsBestWidth();
			UpdateDisplay();

			SendExpanderEvent(EvtITEM_COLLAPSED, node);
		}
	}

	void MainWindow::ToggleExpand(Row row)
	{
		IsExpanded(row) ? Collapse(row) : Expand(row);
	}
	bool MainWindow::IsExpanded(Row row) const
	{
		if (!IsList())
		{
			Node* node = GetNodeByRow(row);
			if (node && node->HasChildren())
			{
				return node->IsNodeExpanded();
			}
		}
		return false;
	}
	bool MainWindow::HasChildren(Row row) const
	{
		if (!m_IsRepresentingList)
		{
			Node* node = GetNodeByRow(row);
			return node && node->HasChildren();
		}
		return false;
	}

	Node* MainWindow::GetNodeByRow(Row row) const
	{
		if (m_IsVirtualListModel)
		{
			if (row < GetRowCount())
			{
				VirtualNode& node = const_cast<VirtualNode&>(m_VirtualNode);
				node.SetVirtualRow(row);
				return &node;
			}
		}
		else if (row)
		{
			NodeOperation_RowToNode operation(row, -2);
			operation.Walk(const_cast<RootNode&>(m_TreeRoot));
			return operation.GetResult();
		}
		return nullptr;
	}
	Row MainWindow::GetRowByNode(const Node& node) const
	{
		if (m_IsVirtualListModel)
		{
			return m_VirtualNode.GetVirtualRow();
		}
		else if (m_Model)
		{
			Row row = 0;
			const Node* currentNode = &node;
			while (currentNode && !currentNode->IsRootNode())
			{
				// Add current node sub row index
				row += currentNode->GetIndexWithinParent() + 1;

				// If this node has parent, add subtree count from all previous siblings
				if (const Node* parentNode = currentNode->GetParent())
				{
					for (const Node* childNode: parentNode->GetChildren())
					{
						if (childNode != currentNode)
						{
							row += childNode->GetSubTreeCount();
						}
						else
						{
							break;
						}
					}
					currentNode = parentNode;
				}
			}

			// If we reached root node, consider search successful.
			if (currentNode && currentNode->IsRootNode())
			{
				// Rows are zero-based, but we calculated it as one-based.
				return row - 1;
			}
		}
		return Row();
	}

	bool MainWindow::BeginEdit(Node& node, Column& column)
	{
		// Cancel any previous editing
		CancelEdit();

		Editor* editor = node.GetEditor(column);
		if (editor)
		{
			m_View->EnsureVisible(node, &column);
			m_View->SetFocus();

			const wxRect itemRect = GetItemRect(node, &column);
			if (editor->BeginEdit(node, column, itemRect))
			{
				// Save the renderer to be able to finish/cancel editing it later.
				m_CurrentEditor = editor;
				return true;
			}
		}
		return false;
	}
	void MainWindow::EndEdit()
	{
		if (m_CurrentEditor)
		{
			m_CurrentEditor->EndEdit();
			m_CurrentEditor = nullptr;
		}
	}
	void MainWindow::CancelEdit()
	{
		if (m_CurrentEditor)
		{
			m_CurrentEditor->CancelEdit();
			m_CurrentEditor = nullptr;
		}
	}
}
