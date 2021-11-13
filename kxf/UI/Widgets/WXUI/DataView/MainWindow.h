#pragma once
#include "Common.h"
#include "View.h"
#include "../../../DataView/Row.h"
#include "../../../DataView/Node.h"
#include "../../../DataView/Column.h"
#include "../../../DataView/SortMode.h"
#include "../../../DataView/CellState.h"
#include "../../../Events/DataViewWidgetEvent.h"

#include "Renderers/NullRenderer.h"
#include "kxf/UI/Windows/ToolTipEx.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "kxf/EventSystem/Event.h"
#include <wx/selstore.h>

namespace kxf::DataView
{
	class Column;
	class ToolTip;
}
namespace kxf::WXUI::DataView
{
	class HeaderCtrl;
	class MainWindow;
	class DropSource;
	class DropTarget;
}

namespace kxf::WXUI::DataView
{
	class KX_API MainWindow: public UI::WindowRefreshScheduler<wxWindow>
	{
		friend class DV::Node;
		friend class DV::Column;
		friend class DV::ToolTip;

		friend class Renderer;
		friend class Editor;
		friend class View;
		friend class HeaderCtrl;
		friend class HeaderCtrl2;

		friend class DropSource;
		friend class DropTarget;
		friend class MaxWidthCalculator;

		public:
			enum: size_t
			{
				INVALID_ROW = std::numeric_limits<size_t>::max(),
				INVALID_COLUMN = std::numeric_limits<size_t>::max(),
				INVALID_COUNT = std::numeric_limits<size_t>::max(),
			};
			enum class InteractibleCell
			{
				Editor,
				Activator,
			};
			enum class UniformHeight
			{
				Default,
				ListView,
				Explorer
			};

		private:
			View* m_View = nullptr;

			int m_UniformRowHeight = 0;
			int m_Indent = 0;

			std::shared_ptr<IGraphicsRenderer> m_GraphicsRenderer;
			NullRenderer m_NullRenderer;
			Column* m_CurrentColumn = nullptr;
			Row m_CurrentRow;
			wxSelectionStore m_SelectionStore;

			bool m_LastOnSame = false;
			bool m_HasFocus = false;
			bool m_UseCellFocus = false;
			bool m_IsCurrentColumnSetByKeyboard = false;
			bool m_IsMouseOverExpander = false;

			bool m_Dirty = true;
			bool m_RedrawNeeded = false;

			DV::Row m_HotTrackRow;
			bool m_HotTrackRowEnabled = false;
			DV::Column* m_HotTrackColumn = nullptr;

			// Tooltip
			UI::ToolTipEx m_ToolTip;
			wxTimer m_ToolTipTimer;

			// Drag and Drop
			DnDInfo m_DragDropInfo;
			wxDataObjectComposite* m_DragDropDataObject = nullptr;
			DropTarget* m_DropTarget = nullptr;
			DropSource* m_DragSource = nullptr;
			size_t m_DragCount = 0;

			Point m_DragStart;
			DV::Row m_DropHintRow;
			bool m_DropHint = false;

			// Double click logic
			DV::Row m_RowLastClicked;
			DV::Row m_RowBeforeLastClicked;
			DV::Row m_RowSelectSingleOnUp;

			// The pen used to draw horizontal/vertical rules
			std::shared_ptr<IGraphicsPen> m_PenRuleH;
			std::shared_ptr<IGraphicsPen> m_PenRuleV;

			// The pen used to draw the expander and the lines
			std::shared_ptr<IGraphicsPen> m_PenExpander;

			// Background bitmap
			BitmapImage m_BackgroundBitmap;
			FlagSet<Alignment> m_BackgroundBitmapAlignment = Alignment::Invalid;
			bool m_FitBackgroundBitmap = false;

			// Make 'm_ItemsCount' = -1 will cause the class recalculate the real displaying number of rows.
			size_t m_ItemsCount = INVALID_COUNT;
			std::shared_ptr<Model> m_Model;
			DV::RootNode* m_TreeRoot = nullptr;

			// String to display when the control is empty
			String m_EmptyControlLabel;

			// This is the tree node under the cursor
			DV::Node* m_TreeNodeUnderMouse = nullptr;

			// Current editor
			Editor* m_CurrentEditor = nullptr;

		protected:
			// Events
			void OnChar(wxKeyEvent& event);
			void OnCharHook(wxKeyEvent& event);
			void OnVerticalNavigation(const wxKeyEvent& event, int delta);
			void OnLeftKey(wxKeyEvent& event);
			void OnRightKey(wxKeyEvent& event);

			void OnMouse(wxMouseEvent& event);
			void OnSetFocus(wxFocusEvent& event);
			void OnKillFocus(wxFocusEvent& event);

			void OnTooltipEvent(wxTimerEvent& event);

			// Return false only if the event was vetoed by its handler.
			bool SendExpanderEvent(const EventID& type, DV::Node& item);
			void SendSelectionChangedEvent(DV::Node* item, DV::Column* column = nullptr);

			// Will return true if event allowed
			bool SendEditingStartedEvent(DV::Node& item, Editor* editor);
			bool SendEditingDoneEvent(DV::Node& item, Editor* editor, bool canceled, const Any& value);

			// Drawing
			void OnPaint(wxPaintEvent& event);
			DV::CellState GetCellStateForRow(DV::Row row) const;

			void UpdateDisplay();
			void RefreshDisplay();
			void RecalculateDisplay();
			void DoSetVirtualSize(int x, int y) override;

			// Tooltip
			bool ShowToolTip(const DV::Node& node, DV::Column& column);
			void RemoveTooltip();

			// Columns
			void OnDeleteColumn(DV::Column& column);
			void OnColumnCountChanged();
			bool IsCellInteractible(const DV::Node& node, const DV::Column& column, InteractibleCell action) const;
			DV::Column* FindInteractibleColumn(const DV::Node& node, InteractibleCell action);
			int CalcBestColumnWidth(DV::Column& column) const;
			bool FitLastColumn(bool update = true);

			// Items
			void InvalidateItemCount();
			void UpdateItemCount(size_t count)
			{
				m_ItemsCount = count;
				m_SelectionStore.SetItemCount(count);
			}
			size_t RecalculateItemCount();

			void OnCellChanged(DV::Node& node, DV::Column* column);
			void OnNodeAdded(DV::Node& node);
			void OnNodeRemoved(DV::Node& node, intptr_t removedCount);
			void OnItemsCleared();
			void OnShouldResort();

			void DoAssignModel(std::shared_ptr<Model> model);
			bool IsListLike() const;

			// Misc
			void OnInternalIdle() override;

		public:
			MainWindow(View* parent, wxWindowID id);
			~MainWindow();

		public:
			void CreateEventTemplate(DataViewWidgetEvent& event, DV::Node* node = nullptr, DV::Column* column = nullptr);

			// Model and nodes
			void AssignModel(std::shared_ptr<IDataViewModel> model)
			{
				DoAssignModel(std::move(model));
			}

			const DV::RootNode& GetRootNode() const
			{
				return *m_TreeRoot;
			}
			DV::RootNode& GetRootNode()
			{
				return *m_TreeRoot;
			}
			void ItemsChanged();

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
			const BitmapImage& GetBackgroundBitmap() const
			{
				return m_BackgroundBitmap;
			}
			void SetBackgroundBitmap(const BitmapImage& bitmap, FlagSet<Alignment> align = Alignment::Invalid, bool fit = false)
			{
				m_BackgroundBitmap = bitmap;
				m_BackgroundBitmapAlignment = align != Alignment::Invalid ? align : Alignment::None;
				m_FitBackgroundBitmap = fit;

				Refresh();
			}

			// Refreshing
			void RefreshRow(DV::Row row)
			{
				RefreshRows(row, row);
			}
			void RefreshRows(DV::Row from, DV::Row to);
			void RefreshRowsAfter(DV::Row firstRow);
			void RefreshColumn(const DV::Column& column);

			// Item rect
			Rect GetRowRect(DV::Row row) const;
			Rect GetRowsRect(DV::Row rowFrom, DV::Row rowTo) const;

			int GetRowStart(DV::Row row) const;
			int GetRowHeight(DV::Row row) const;
			int GetVariableRowHeight(const DV::Node& node) const;
			int GetVariableRowHeight(DV::Row row) const;
			int GetRowWidth() const;
			DV::Row GetRowAt(int yCoord) const;

			int GetUniformRowHeight() const
			{
				return m_UniformRowHeight;
			}
			void SetUniformRowHeight(int height);
			int GetDefaultRowHeight(UniformHeight type) const;

			// Drag and Drop
			GDIBitmap CreateItemBitmap(DV::Row row, int& indent);
			bool EnableDND(std::unique_ptr<wxDataObjectSimple> dataObject, DNDOpType type, bool isPreferredDrop = false);
			bool DisableDND(const wxDataFormat& format);

			std::tuple<DV::Row, DV::Node*> DragDropHitTest(const Point& pos) const;
			void RemoveDropHint();
			wxDragResult OnDragOver(const wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult);
			wxDragResult OnDropData(wxDataObjectSimple& dataObject, const Point& pos, wxDragResult dragResult);
			bool TestDropPossible(const wxDataObjectSimple& dataObject, const Point& pos);

			wxDragResult OnDragDropEnter(const wxDataObjectSimple& format, const Point& pos, wxDragResult dragResult);
			void OnDragDropLeave();

			// Scrolling
			void ScrollWindow(int dx, int dy, const wxRect* rect = nullptr) override;
			void ScrollTo(DV::Row row, size_t column = INVALID_COLUMN);
			void EnsureVisible(DV::Row row, size_t column = INVALID_COLUMN);

			// Current row and column
			DV::Row GetCurrentRow() const
			{
				return m_CurrentRow;
			}
			bool HasCurrentRow()
			{
				return !m_CurrentRow.IsNull();
			}
			void ChangeCurrentRow(DV::Row row);
			bool TryAdvanceCurrentColumn(DV::Node* node, wxKeyEvent& event, bool moveForward);

			DV::Column* GetCurrentColumn() const
			{
				return m_CurrentColumn;
			}
			void ClearCurrentColumn()
			{
				m_CurrentColumn = nullptr;
			}

			DV::Node* GetHotTrackItem() const;
			DV::Column* GetHotTrackColumn() const;

			// Selection
			bool IsSingleSelection() const
			{
				return !IsMultipleSelection();
			}
			bool IsMultipleSelection() const
			{
				return m_View->ContainsWindowStyle(CtrlStyle::MultipleSelection);
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
			bool UnselectAllRows(DV::Row exceptThisRow = {});
			void ReverseRowSelection(DV::Row row);
			void ClearSelection()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1, false);
			}
			void SelectRow(DV::Row row, bool select = true);
			void SelectRows(DV::Row from, DV::Row to);
			void SelectAllRows()
			{
				m_SelectionStore.SelectRange(0, GetRowCount() - 1);
				Refresh();
			}
			bool IsRowSelected(DV::Row row) const
			{
				return m_SelectionStore.IsSelected(*row);
			}

			// View
			IGraphicsRenderer& GetGraphicsRenderer()
			{
				return *m_GraphicsRenderer;
			}
			void SetRuleHPen(const Color& color, float width = 1.0f)
			{
				m_PenRuleH = m_GraphicsRenderer->CreatePen(color, width);
			}
			void SetRuleVPen(const Color& color, float width = 1.0f)
			{
				m_PenRuleV = m_GraphicsRenderer->CreatePen(color, width);
			}

			size_t GetRowCount() const;
			size_t GetCountPerPage() const;
			DV::Row GetFirstVisibleRow() const;
			DV::Row GetLastVisibleRow() const;

			void HitTest(const Point& pos, DV::Node** nodeOut = nullptr, DV::Column** columnOut = nullptr);
			void HitTest(const Point& pos, DV::Node*& node, DV::Column*& column)
			{
				return HitTest(pos, &node, &column);
			}
			DV::Node* HitTestNode(const Point& pos)
			{
				Node* node = nullptr;
				HitTest(pos, &node);
				return node;
			}
			DV::Column* HitTestColumn(const Point& pos)
			{
				Column* column = nullptr;
				HitTest(pos, nullptr, &column);
				return column;
			}
			Rect GetItemRect(const DV::Node& item, const DV::Column* column = nullptr);

			// Rows
			void Expand(DV::Row row);
			void Expand(DV::Node& node, DV::Row row = {});
			void Collapse(DV::Row row);
			void Collapse(DV::Node& node, DV::Row row = {});
			void ToggleExpand(DV::Row row);
			bool IsExpanded(DV::Row row) const;
			bool HasChildren(DV::Row row) const;

			DV::Node* GetNodeByRow(DV::Row row) const;
			DV::Row GetRowByNode(const DV::Node& item) const;

			// Editing
			bool BeginEdit(DV::Node& item, DV::Column& column);
			void EndEdit();
			void CancelEdit();
	};
}
