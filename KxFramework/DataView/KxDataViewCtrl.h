#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewItem.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
class KX_API KxDataViewHeaderCtrl;
class KX_API KxDataViewMainWindow;
class KX_API KxMenu;

class KX_API KxDataViewCtrl: public wxSystemThemedControl<wxControl>, public wxScrollHelper
{
	friend class KxDataViewHeaderCtrl;
	friend class KxDataViewMainWindow;
	friend class KxDataViewColumn;
	friend class KxDataViewRenderer;
	friend class KxDataViewEditor;

	public:
		enum: size_t
		{
			INVALID_ROW = (size_t)-1,
			INVALID_COLUMN = (size_t)-1,
			INVALID_COUNT = (size_t)-1,
		};

		template<class RendererT, class EditorT>
		class AddedColumnInfo
		{
			private:
				KxDataViewColumn* m_Column = NULL;
				KxDataViewRenderer* m_Renderer = NULL;
				KxDataViewEditor* m_Editor = NULL;

			public:
				AddedColumnInfo(KxDataViewColumn* column, KxDataViewRenderer* renderer, KxDataViewEditor* editor = NULL)
					:m_Column(column), m_Renderer(renderer), m_Editor(editor)
				{
				}

			public:
				KxDataViewColumn* GetColumn() const
				{
					return m_Column;
				}
				RendererT* GetRenderer() const
				{
					return static_cast<RendererT*>(m_Renderer);
				}
				typename std::conditional<std::is_same<EditorT, void>::value, void, EditorT*>::type GetEditor() const
				{
					if constexpr(!std::is_same<EditorT, void>::value)
					{
						return static_cast<EditorT*>(m_Editor);
					}
					else
					{
						static_assert(false, "This column has no editor");
					}
				}
		};

	private:
		KxDataViewModel* m_Model = NULL;
		KxDataViewColumn* m_ExpanderColumn = NULL;

		KxDataViewHeaderCtrl* m_HeaderArea = NULL;
		KxDataViewMainWindow* m_ClientArea = NULL;
		KxDataViewModelNotifier* m_Notifier = NULL;

		wxBoxSizer* m_Sizer = NULL;
		wxSizerItem* m_HeaderAreaSI = NULL;
		wxSizerItem* m_ClientAreaSI = NULL;

		int m_Indent = 0;
		bool m_UsingSystemTheme = false;
		KxColor m_BorderColor;

		// If true, allow sorting by more than one column
		bool m_AllowMultiColumnSort = false;

		// User defined color to draw row lines, may be invalid
		KxColor m_AlternateRowColor;

		// Columns storage
		class ColumnInfo
		{
			private:
				KxDataViewColumn* m_Column = NULL;
				size_t m_BestWidth = 0;
				bool m_Dirty = true;

			public:
				ColumnInfo(KxDataViewColumn* column)
					:m_Column(column)
				{
				}

			public:
				KxDataViewColumn* GetColumn() const
				{
					return m_Column;
				}

				bool HasBestWidth() const
				{
					return m_BestWidth != 0;
				}
				size_t GetBestWidth() const
				{
					return m_BestWidth;
				}
				void SetBestWidth(size_t width)
				{
					m_BestWidth = width;
				}
				void InvalidateBestWidth()
				{
					m_BestWidth = 0;
					MarkDirty();
				}

				bool IsDirty() const
				{
					return m_Dirty;
				}
				void MarkDirty(bool value = true)
				{
					m_Dirty = value;
				}
		};
		std::vector<std::unique_ptr<ColumnInfo>> m_Columns;

		// This indicates that at least one entry in 'm_Columns' has 'm_Dirty'
		// flag set. It's cheaper to check one flag in OnInternalIdle() than to
		// iterate over 'm_Columns' to check if anything needs to be done.
		bool m_ColumnsDirty = false;

		// Columns display order
		KxIntPtrVector m_ColumnsOrder;

		// Columns indices used for sorting, empty if nothing is sorted
		KxIntPtrVector m_ColumnsSortingIndexes;

	private:
		// Implementation of the public Set/GetCurrentItem() methods which are only
		// called in multi selection case (for single selection controls their
		// implementation is trivial and is done in the base class itself).
		virtual KxDataViewItem DoGetCurrentItem() const;
		virtual void DoSetCurrentItem(const KxDataViewItem& item);

		void InvalidateColBestWidths();
		void InvalidateColBestWidth(size_t index);
		void UpdateColWidths();
		void UpdateColumnsOrderArray();

		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event);
		virtual wxSize GetSizeAvailableForScrollTarget(const wxSize& size);

	protected:
		virtual void DoEnable(bool value) override;
		virtual void DoSetExpanderColumn();
		virtual void DoSetIndent();
		virtual void DoInsertColumn(KxDataViewColumn* column, size_t position);
		virtual void EnsureVisible(size_t row, size_t column);

		auto GetColumnIterator(const KxDataViewColumn* column) const
		{
			return std::find_if(m_Columns.begin(), m_Columns.end(), [column](const auto& value)
			{
				return value->GetColumn() == column;
			});
		}

		// Notice that row here may be invalid (i.e. >= GetRowCount()), this is not
		// an error and this function simply returns an invalid item in this case.
		virtual KxDataViewItem GetItemByRow(size_t row) const;
		virtual size_t GetRowByItem(const KxDataViewItem & item) const;

		// Mark the column as being used or not for sorting.
		void UseColumnForSorting(size_t index);
		void DontUseColumnForSorting(size_t index);

		// Return true if the given column is sorted
		bool IsColumnSorted(size_t index) const;

		// Reset all columns currently used for sorting.
		void ResetAllSortColumns();

		// We need to return a special WM_GETDLGCODE value to process just the
		// arrows but let the other navigation characters through
		virtual WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
		virtual void OnInternalIdle() override;
		virtual void DoEnableSystemTheme(bool enable, wxWindow* window) override;

	public:
		static const KxDataViewCtrlStyles DefaultStyle = KxDV_SINGLE_SELECTION|KxDV_DOUBLE_CLICK_EXPAND;

		KxDataViewCtrl()
			:wxScrollHelper(this)
		{
		}
		KxDataViewCtrl(wxWindow*parent,
					   wxWindowID id,
					   long style = DefaultStyle,
					   const wxValidator& validator = wxDefaultValidator
		)
			:wxScrollHelper(this)
		{
			Create(parent, id, style, validator);
		}
		bool Create(wxWindow*parent,
					wxWindowID id,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);
		virtual ~KxDataViewCtrl();

	public:
		/* Model */
		KxDataViewModel* GetModel()
		{
			return m_Model;
		}
		const KxDataViewModel* GetModel() const
		{
			return m_Model;
		}
		
		bool AssociateModel(KxDataViewModel* model);
		bool AssignModel(KxDataViewModel* model);

		KxDataViewModelNotifier* GetNotifier() const
		{
			return m_Notifier;
		}

		/* Column management */
		KxDataViewRenderer* AppendColumn(KxDataViewColumn* column);
		KxDataViewRenderer* PrependColumn(KxDataViewColumn* column);
		KxDataViewRenderer* InsertColumn(size_t pos, KxDataViewColumn* column);

		template<class RendererT, class EditorT = void>
		AddedColumnInfo<RendererT, EditorT> AppendColumn(const wxString& title, int id, KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int width = KxCOL_WIDTH_AUTOSIZE, KxDataViewColumnFlags flags = KxDV_COL_DEFAULT_FLAGS)
		{
			RendererT* renderer = new RendererT(cellMode);
			KxDataViewColumn* column = new KxDataViewColumn(title, renderer, id);
			column->SetWidth(width);
			column->SetFlags(flags);

			if constexpr(!std::is_same<EditorT, void>::value)
			{
				column->AssignEditor(new EditorT());
			}
			AppendColumn(column);

			return AddedColumnInfo<RendererT, EditorT>(column, renderer, column->GetEditor());
		}

		template<class RendererT, class EditorT = void>
		AddedColumnInfo<RendererT, EditorT> PrependColumn(const wxString& title, int id, KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int width = KxCOL_WIDTH_AUTOSIZE, KxDataViewColumnFlags flags = KxDV_COL_DEFAULT_FLAGS)
		{
			RendererT* renderer = new RendererT(cellMode);
			KxDataViewColumn* column = new KxDataViewColumn(title, renderer, id);
			column->SetWidth(width);
			column->SetFlags(flags);

			if constexpr(!std::is_same<EditorT, void>::value)
			{
				column->AssignEditor(new EditorT());
			}
			PrependColumn(column);

			return AddedColumnInfo<RendererT, EditorT>(column, renderer, column->GetEditor());
		}

		template<class RendererT, class EditorT = void>
		AddedColumnInfo<RendererT, EditorT> InsertColumn(size_t pos, const wxString& title, int id, KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int width = KxCOL_WIDTH_AUTOSIZE, KxDataViewColumnFlags flags = KxDV_COL_DEFAULT_FLAGS)
		{
			RendererT* renderer = new RendererT(cellMode);
			KxDataViewColumn* column = new KxDataViewColumn(title, renderer, id);
			column->SetWidth(width);
			column->SetFlags(flags);

			if constexpr(!std::is_same<EditorT, void>::value)
			{
				column->AssignEditor(new EditorT());
			}
			InsertColumn(pos, column);
			
			return AddedColumnInfo<RendererT, EditorT>(column, renderer, column->GetEditor());
		}

		size_t GetColumnCount() const
		{
			return m_Columns.size();
		}
		size_t GetVisibleColumnCount() const;
		size_t GetHiddenColumnCount() const
		{
			return GetColumnCount() - GetVisibleColumnCount();
		}
		const KxIntPtrVector& GetColumnsOrder() const
		{
			return m_ColumnsOrder;
		}

		KxDataViewColumn* GetColumn(size_t position) const;
		KxDataViewColumn* GetColumnByID(int id) const
		{
			size_t pos = GetColumnIndexByID(id);
			return pos != INVALID_COLUMN ? GetColumn(pos) : NULL;
		}
		size_t GetColumnPosition(const KxDataViewColumn* column) const;

		bool DeleteColumn(KxDataViewColumn *column);
		bool ClearColumns();

		void SetExpanderColumn(KxDataViewColumn *column)
		{
			m_ExpanderColumn = column;
			DoSetExpanderColumn();
		}
		KxDataViewColumn* GetExpanderColumn() const
		{
			return m_ExpanderColumn;
		}
		
		// Return the expander column or, if it is not set, the first column and also
		// set it as the expander one for the future.
		KxDataViewColumn* GetExpanderColumnOrFirstOne();

		bool IsMultiColumnSortUsed() const
		{
			return m_ColumnsSortingIndexes.size() > 1;
		}
		KxDataViewColumn* GetSortingColumn() const;
		KxDataViewColumn::Vector GetSortingColumns() const;

		// This must be overridden to return true if the control does allow sorting
		// by more than one column, which is not the case by default.
		bool AllowMultiColumnSort(bool allow);

		// This should also be overridden to actually use the specified column for
		// sorting if using multiple columns is supported.
		void ToggleSortByColumn(size_t position);

		// Currently focused column of the current item or NULL if no column has focus
		virtual KxDataViewColumn* GetCurrentColumn() const;

		virtual bool IsMultiColumnSortAllowed() const
		{
			return m_AllowMultiColumnSort;
		}

		/* Items management */
		int GetIndent() const
		{
			return m_Indent;
		}
		void SetIndent(int indent)
		{
			m_Indent = indent;
			DoSetIndent();
		}

		// Current item is the one used by the keyboard navigation, it is the same
		// as the (unique) selected item in single selection mode so these
		// functions are mostly useful for controls with KxDV_MULTIPLE style.
		KxDataViewItem GetCurrentItem() const;
		void SetCurrentItem(const KxDataViewItem& item);

		// Selection: both GetSelection() and GetSelections() can be used for the
		// controls both with and without KxDV_MULTIPLE style. For single selection
		// controls GetSelections() is not very useful however. And for multi
		// selection controls GetSelection() returns an invalid item if more than
		// one item is selected. Use GetSelectedItemsCount() or HasSelection() to
		// check if any items are selected at all.
		int GetSelectedItemsCount() const;
		bool HasSelection() const
		{
			return GetSelectedItemsCount() != 0;
		}
		KxDataViewItem GetSelection() const;
		size_t GetSelections(KxDataViewItem::Vector& sel) const;
		void SetSelections(const KxDataViewItem::Vector& sel);
		void Select(const KxDataViewItem& item);
		void Unselect(const KxDataViewItem& item);
		bool IsSelected(const KxDataViewItem& item) const;

		KxDataViewItem GetHotTrackedItem() const;
		KxDataViewColumn* GetHotTrackedColumn() const;

		void GenerateSelectionEvent(const KxDataViewItem& item, const KxDataViewColumn* column = NULL);

		void SelectAll();
		void UnselectAll();

		void Expand(const KxDataViewItem& item);
		void ExpandAncestors(const KxDataViewItem& item);
		void Collapse(const KxDataViewItem& item);
		bool IsExpanded(const KxDataViewItem& item) const;
		void SetItemExpanded(const KxDataViewItem& item, bool expanded)
		{
			if (expanded)
			{
				Expand(item);
			}
			else
			{
				Collapse(item);
			}
		}
		void ToggleItemExpanded(const KxDataViewItem& item)
		{
			SetItemExpanded(item, !IsExpanded(item));
		}

		void EnsureVisible(const KxDataViewItem& item, const KxDataViewColumn* column = NULL);
		void HitTest(const wxPoint& point, KxDataViewItem& item, KxDataViewColumn*& column) const;
		wxRect GetItemRect(const KxDataViewItem& item, const KxDataViewColumn* column = NULL) const;
		wxRect GetAdjustedItemRect(const KxDataViewItem& item, const KxDataViewColumn* column = NULL) const;
		wxPoint GetDropdownMenuPosition(const KxDataViewItem& item, const KxDataViewColumn* column = NULL) const;

		int GetUniformRowHeight() const;
		void SetUniformRowHeight(int rowHeight);
		int GetDefaultRowHeight(KxDataViewDefaultRowHeightType type = KxDVC_ROW_HEIGHT_DEFAULT) const;

		bool EditItem(const KxDataViewItem& item, const KxDataViewColumn* column);

		/* Drag and drop */
		bool EnableDragSource(const wxDataFormat& format);
		bool EnableDropTarget(const wxDataFormat& format);

		/* Window */
		virtual void SetFocus() override;
		virtual bool SetFont(const wxFont& font) override;

		bool HasHeaderCtrl() const
		{
			return m_HeaderArea != NULL;
		}
		wxHeaderCtrl* GetHeaderCtrl();
		const wxHeaderCtrl* GetHeaderCtrl() const;

		KxDataViewMainWindow* GetMainWindow()
		{
			return m_ClientArea;
		}
		const KxDataViewMainWindow* GetMainWindow() const
		{
			return m_ClientArea;
		}

		bool CreateColumnSelectionMenu(KxMenu& tMenu);
		KxDataViewColumn* OnColumnSelectionMenu(KxMenu& tMenu);

		/* Control visuals */
		static wxVisualAttributes GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL)
		{
			return wxControl::GetCompositeControlsDefaultAttributes(variant);
		}
		virtual wxVisualAttributes GetDefaultAttributes() const override
		{
			return GetClassDefaultAttributes(GetWindowVariant());
		}
		virtual wxBorder GetDefaultBorder() const override;

		KxColor GetAlternateRowColor() const
		{
			return m_AlternateRowColor;
		}
		void SetAlternateRowColor(const KxColor& color)
		{
			m_AlternateRowColor = color;
		}

		KxColor GetBorderColor() const
		{
			return m_BorderColor;
		}
		void SetBorderColor(const KxColor& color, int size = 1);

		/* Utility functions, not part of the API */

		// Returns the "best" width for the index-th column
		size_t GetBestColumnWidth(size_t index) const;

		// Called by header window after reorder
		void ColumnMoved(KxDataViewColumn* column, size_t new_pos);

		// Update the display after a change to an individual column
		void OnColumnChange(size_t index);

		// Update after a change to the number of columns
		void OnColumnsCountChanged();

		// Return the index of the given column in m_cols
		size_t GetColumnIndex(const KxDataViewColumn* column) const;

		// Return the index of the column having the given model index.
		size_t GetColumnIndexByID(int columnID) const;

		// Return the column displayed at the given position in the control
		KxDataViewColumn* GetColumnAt(size_t position) const;

		// Return the column displayed at the given position in the control.
		// Only for currently visible columns.
		KxDataViewColumn* GetColumnAtVisible(size_t position) const;

	public:
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxDataViewCtrl);

	private:
		WX_FORWARD_TO_SCROLL_HELPER();
};
