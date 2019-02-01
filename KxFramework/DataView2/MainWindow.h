#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "View.h"
#include "Node.h"
#include "Row.h"
#include "Model.h"
#include "CellState.h"
#include "SortOrder.h"
#include "Renderers/NullRenderer.h"
#include <wx/selstore.h>

namespace KxDataView2
{
	class KX_API View;
	class KX_API Column;
	class KX_API Renderer;
	class KX_API Editor;
	class KX_API HeaderCtrl;
	class KX_API Event;
	class KX_API VirtualListModel;
}

namespace KxDataView2
{
	class KX_API MainWindow: public wxWindow
	{
		friend class KX_API Renderer;
		friend class KX_API Editor;
		friend class KX_API Column;
		friend class KX_API Event;
		friend class KX_API View;
		friend class KX_API Node;
		friend class KX_API HeaderCtrl;
		friend class KX_API VirtualListModel;
		friend class MaxWidthCalculator;

		public:
			enum: size_t
			{
				INVALID_ROW = (size_t)-1,
				INVALID_COLUMN = (size_t)-1,
				INVALID_COUNT = (size_t)-1,
			};
			enum: int
			{
				// Cell padding on the left/right
				PADDING_RIGHTLEFT = 3,

				// Expander space margin
				EXPANDER_MARGIN = 4,
				EXPANDER_OFFSET = 4,
			};
			enum class InteractibleCell
			{
				Editor,
				Activator,
			};

		private:
			View* m_View = nullptr;
			Model* m_Model = nullptr;
			bool m_OwnModel = false;
			bool m_IsRepresentingList = false;
			bool m_IsVirtualListModel = false;

			int m_UniformRowHeight = 0;
			int m_Indent = 0;
			bool m_Dirty = true;

			NullRenderer m_NullRenderer;
			Column* m_CurrentColumn = nullptr;
			Row m_CurrentRow;
			wxSelectionStore m_SelectionStore;

			bool m_LastOnSame = false;
			bool m_HasFocus = false;
			bool m_UseCellFocus = false;
			bool m_IsCurrentColumnSetByKeyboard = false;
			bool m_IsMouseOverExpander = false;

			Row m_HotTrackRow;
			bool m_HotTrackRowEnabled = false;
			Column* m_HotTrackColumn = nullptr;

			// Drag and Drop
			size_t m_DragCount = 0;
			wxPoint m_DragStart = wxPoint(0, 0);

			bool m_DragEnabled = false;
			wxDataFormat m_DragFormat;

			bool m_DropEnabled = false;
			wxDataFormat m_DropFormat;
			bool m_DropHint = false;
			Row m_DropHintLine;

			// Double click logic
			Row m_RowLastClicked;
			Row m_RowBeforeLastClicked;
			Row m_RowSelectSingleOnUp;

			// The pen used to draw horizontal/vertical rules
			wxPen m_PenRuleH;
			wxPen m_PenRuleV;

			// the pen used to draw the expander and the lines
			wxPen m_PenExpander;

			// Background bitmap
			wxBitmap m_BackgroundBitmap;
			wxAlignment m_BackgroundBitmapAlignment = wxALIGN_INVALID;
			bool m_FitBackgroundBitmap = false;

			// This is the tree structure of the model.
			// Make 'm_ItemsCount' = -1 will cause the class recalculate the real displaying number of rows.
			std::unique_ptr<Node> m_TreeRoot;
			size_t m_ItemsCount = INVALID_COUNT;

			// This is the tree node under the cursor
			Node* m_TreeNodeUnderMouse = nullptr;

			// Current editor
			Editor* m_CurrentEditor = nullptr;

		protected:
			/* Events */
			void OnChar(wxKeyEvent& event);
			void OnCharHook(wxKeyEvent& event);
			void OnVerticalNavigation(const wxKeyEvent& event, int delta);
			void OnLeftKey(wxKeyEvent& event);
			void OnRightKey(wxKeyEvent& event);
			void OnMouse(wxMouseEvent& event);
			void OnSetFocus(wxFocusEvent& event);
			void OnKillFocus(wxFocusEvent& event);

			// Return false only if the event was vetoed by its handler.
			bool SendExpanderEvent(wxEventType type, Node& item);
			void SendSelectionChangedEvent(Node* item, Column* column = nullptr);

			// Will return true if event allowed
			bool SendEditingStartedEvent(Node& item, Editor* editor);
			bool SendEditingDoneEvent(Node& item, Editor* editor, bool canceled, const wxAny& value);

			/* Drawing */
			void OnPaint(wxPaintEvent& event);
			CellState GetCellStateForRow(Row row) const;

			SortOrder GetSortOrder() const
			{
				Column* column = m_View->GetSortingColumn();
				if (column)
				{
					return SortOrder::UseColumn(*column, column->IsSortedAscending());
				}
				return SortOrder::UseNone();
			}
			void UpdateDisplay();
			void RecalculateDisplay();

			/* Columns */
			void OnColumnCountChanged();
			bool IsCellInteractible(const Node& node, const Column& column, InteractibleCell action) const;
			Column* FindInteractibleColumn(const Node& node, InteractibleCell action);
			int CalcBestColumnWidth(Column& column) const;

			/* Items */
			void InvalidateItemCount()
			{
				m_ItemsCount = INVALID_COUNT;
			}
			void UpdateItemCount(size_t count)
			{
				m_ItemsCount = count;
				m_SelectionStore.SetItemCount(count);
			}
			size_t RecalculateItemCount() const;

			void OnCellChanged(Node& node, Column* column);
			void OnNodeAdded(Node& node);
			void OnNodeRemoved(Node& node, intptr_t removedCount);
			void OnItemsCleared();
			void OnShouldResort();

			void BuildTree();
			void DestroyTree();
			void DoAssignModel(Model* model, bool own);
			bool IsRepresentingList() const;

			// Misc
			virtual void OnInternalIdle() override;

		public:
			MainWindow(View* parent, wxWindowID id);
			virtual ~MainWindow();

		public:
			void CreateEventTemplate(Event& event, Node* node = nullptr, Column* column = nullptr);

			// Model and nodes
			bool IsList() const
			{
				return m_IsVirtualListModel || m_IsRepresentingList;
			}
			bool IsVirtualList() const
			{
				return m_IsVirtualListModel;
			}
			
			Model* GetModel() const
			{
				return m_Model;
			}
			void SetModel(Model* model)
			{
				DoAssignModel(model, false);
			}
			void AssignModel(Model* model)
			{
				DoAssignModel(model, true);
			}

			Node& GetRootNode() const
			{
				return *m_TreeRoot;
			}
			void ItemsChanged()
			{
				m_ItemsCount = RecalculateItemCount();
				m_View->InvalidateColumnsBestWidth();
			}

			// View
			void SetView(View* owner)
			{
				m_View = owner;
			}
			View* GetView()
			{
				return m_View;
			}
			const View* GetView() const
			{
				return m_View;
			}

			Renderer& GetNullRenderer()
			{
				return m_NullRenderer;
			}
			const wxBitmap& GetBackgroundBitmap() const
			{
				return m_BackgroundBitmap;
			}
			void SetBackgroundBitmap(const wxBitmap& bitmap, int align = wxALIGN_INVALID, bool fit = false)
			{
				m_BackgroundBitmap = bitmap;
				m_BackgroundBitmapAlignment = static_cast<wxAlignment>(align != wxALIGN_INVALID ? align : wxALIGN_NOT);
				m_FitBackgroundBitmap = fit;
				Refresh();
			}

			// Refreshing
			void RefreshRow(Row row)
			{
				RefreshRows(row, row);
			}
			void RefreshRows(Row from, Row to);
			void RefreshRowsAfter(Row firstRow);
			void RefreshColumn(const Column& column);

			// Item rect
			wxRect GetRowRect(Row row) const;
			wxRect GetRowsRect(Row rowFrom, Row rowTo) const;

			int GetRowStart(Row row) const;
			int GetRowHeight(Row row) const;
			int GetVariableRowHeight(const Node& node) const;
			int GetVariableRowHeight(Row row) const;
			int GetRowWidth() const;
			Row GetRowAt(int yCoord) const;

			int GetUniformRowHeight() const
			{
				return m_UniformRowHeight;
			}
			void SetUniformRowHeight(int height);
			int GetDefaultRowHeight(UniformHeight type = UniformHeight::Default) const;

			// Drag and Drop
			wxBitmap CreateItemBitmap(Row row, int& indent);
			bool EnableDragSource(const wxDataFormat& format);
			bool EnableDropTarget(const wxDataFormat& format);

			void OnDragDropGetRowNode(const wxPoint& pos, Row& row, Node*& item);
			void RemoveDropHint();
			wxDragResult OnDragOver(const wxDataFormat& format, const wxPoint& pos, wxDragResult dragResult);
			wxDragResult OnDragData(const wxDataFormat& format, const wxPoint& pos, wxDragResult dragResult);
			bool OnDrop(const wxDataFormat& format, const wxPoint& pos);
			void OnDragDropLeave();

			// Scrolling
			void ScrollWindow(int dx, int dy, const wxRect* rect = nullptr);
			void ScrollTo(Row row, size_t column = INVALID_COLUMN);
			void EnsureVisible(Row row, size_t column = INVALID_COLUMN);

			// Current row and column
			Row GetCurrentRow() const
			{
				return m_CurrentRow;
			}
			bool HasCurrentRow()
			{
				return m_CurrentRow.IsOK();
			}
			void ChangeCurrentRow(Row row);
			bool TryAdvanceCurrentColumn(Node* node, wxKeyEvent& event, bool moveForward);

			Column* GetCurrentColumn() const
			{
				return m_CurrentColumn;
			}
			void ClearCurrentColumn()
			{
				m_CurrentColumn = nullptr;
			}

			Node* GetHotTrackItem() const;
			Column* GetHotTrackColumn() const;

			// Selection
			bool IsSingleSelection() const
			{
				return !IsMultipleSelection();
			}
			bool IsMultipleSelection() const
			{
				return m_View->IsOptionEnabled(CtrlStyle::MultipleSelection);
			}
			bool IsEmpty()
			{
				return GetRowCount() == 0;
			}

			const wxSelectionStore& GetSelections() const
			{
				return m_SelectionStore;
			}
			bool IsSelectionEmpty() const
			{
				return m_SelectionStore.IsEmpty();
			}

			// If a valid row is specified and it was previously selected, it is left selected and the function
			// returns false. Otherwise, i.e. if there is really no selection left in the control, it returns true.
			bool UnselectAllRows(Row exceptThisRow = {});
			void ReverseRowSelection(Row row);
			void ClearSelection()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1, false);
			}
			void SelectRow(Row row, bool select);
			void SelectRows(Row from, Row to);
			void SelectRows(const Row::Vector& selection);
			void SelectAllRows()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1);
				Refresh();
			}
			bool IsRowSelected(Row row) const
			{
				return m_SelectionStore.IsSelected(row);
			}

			/* View */
			void SetRuleHPen(const wxPen& pen)
			{
				m_PenRuleH = pen;
			}
			void SetRuleVPen(const wxPen& pen)
			{
				m_PenRuleV = pen;
			}

			size_t GetRowCount() const;
			size_t GetCountPerPage() const;
			Row GetFirstVisibleRow() const;
			Row GetLastVisibleRow() const;

			void HitTest(const wxPoint& pos, Node*& item, Column*& column);
			wxRect GetItemRect(const Node& item, const Column* column = nullptr);

			// Adjust last column to window size
			void UpdateColumnSizes();

			// Rows
			void Expand(Row row);
			void Expand(Node& node, Row row = {});
			void Collapse(Row row);
			void Collapse(Node& node, Row row = {});
			void ToggleExpand(Row row);
			bool IsExpanded(Row row) const;
			bool HasChildren(Row row) const;

			Node* GetNodeByRow(Row row) const;
			Row GetRowByNode(const Node& item) const;

			// Editing
			bool BeginEdit(Node& item, Column& column);
			void EndEdit();
			void CancelEdit();

		public:
			wxDECLARE_DYNAMIC_CLASS(MainWindow);
	};
}
namespace KxDataView2
{
	class wxDragImage;
	class DropSource: public wxDropSource
	{
		private:
			MainWindow* m_MainWindow = nullptr;
			wxWindow* m_DragImage = nullptr;
			wxBitmap m_HintBitmap;
			wxPoint m_HintPosition;

			wxPoint m_Distance = wxDefaultPosition;
			size_t m_Row = MainWindow::INVALID_ROW;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnScroll(wxMouseEvent& event);
			virtual bool GiveFeedback(wxDragResult effect) override;

			wxPoint GetHintPosition(const wxPoint& mousePos) const;

		public:
			DropSource(MainWindow* mainWindow, size_t row);
			virtual ~DropSource();
	};
}

namespace KxDataView2
{
	class KX_API DropTarget: public wxDropTarget
	{
		private:
			MainWindow* m_MainWindow = nullptr;

		private:
			virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult dragResult) override;
			virtual bool OnDrop(wxCoord x, wxCoord y) override;
			virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult dragResult) override;
			virtual void OnLeave() override;

		public:
			DropTarget(wxDataObject* dataObject, MainWindow* mainWindow);
	};
}
