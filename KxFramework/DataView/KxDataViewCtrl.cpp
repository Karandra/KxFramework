#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewHeaderCtrl.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/KxMenu.h"

wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewCtrl, wxControl)

WXLRESULT KxDataViewCtrl::MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
{
	WXLRESULT rc = wxControl::MSWWindowProc(msg, wParam, lParam);

	// We need to process arrows ourselves for scrolling
	if (msg == WM_GETDLGCODE)
	{
		rc |= DLGC_WANTARROWS;
	}
	return rc;
}

KxDataViewItem KxDataViewCtrl::DoGetCurrentItem() const
{
	return GetItemByRow(m_ClientArea->GetCurrentRow());
}
void KxDataViewCtrl::DoSetCurrentItem(const KxDataViewItem& item)
{
	const size_t newCurrent = m_ClientArea->GetRowByItem(item);
	const size_t oldCurrent = m_ClientArea->GetCurrentRow();

	if (newCurrent != oldCurrent)
	{
		m_ClientArea->ChangeCurrentRow(newCurrent);
		m_ClientArea->RefreshRow(oldCurrent);
		m_ClientArea->RefreshRow(newCurrent);
	}
}

void KxDataViewCtrl::InvalidateColBestWidths()
{
	// Mark all columns as dirty:
	for (auto& v: m_Columns)
	{
		v->InvalidateBestWidth();
	}
	m_ColumnsDirty = true;
}
void KxDataViewCtrl::InvalidateColBestWidth(size_t index)
{
	m_Columns[index]->InvalidateBestWidth();
	m_ColumnsDirty = true;
}
void KxDataViewCtrl::UpdateColWidths()
{
	m_ColumnsDirty = false;

	if (m_HeaderArea)
	{
		size_t index = 0;
		for (auto& value: m_Columns)
		{
			// Note that we have to have an explicit 'dirty' flag here instead of
			// checking if the width == 0, as is done in GetBestColumnWidth().
			// 
			// Testing width == 0 wouldn't work correctly if some code called
			// GetWidth() after column width invalidation but before
			// KxDataViewCtrl::UpdateColWidths() was called at idle time. This
			// would result in the header's column width getting out of sync with
			// the control itself.

			if (value->IsDirty())
			{
				m_HeaderArea->UpdateColumn(index);
				value->MarkDirty(false);

				index++;
			}
		}
	}
}
void KxDataViewCtrl::UpdateColumnsOrderArray()
{
	if (m_HeaderArea)
	{
		wxArrayInt order = GetHeaderCtrl()->GetColumnsOrder();
		m_ColumnsOrder.assign(order.begin(), order.end());
	}
}

void KxDataViewCtrl::DoEnable(bool value)
{
	wxControl::DoEnable(value);
	Refresh();
}
void KxDataViewCtrl::DoSetExpanderColumn()
{
	KxDataViewColumn* column = GetExpanderColumn();
	if (column)
	{
		size_t index = GetColumnIndex(column);
		if (index  != INVALID_ROW)
		{
			InvalidateColBestWidth(index);
		}
	}
	m_ClientArea->UpdateDisplay();
}
void KxDataViewCtrl::DoSetIndent()
{
	m_ClientArea->UpdateDisplay();
}
void KxDataViewCtrl::DoInsertColumn(KxDataViewColumn* column, size_t position)
{
	// Correct insertion position
	if (m_Columns.empty())
	{
		position = 0;
	}
	else if (position > m_Columns.size())
	{
		position = m_Columns.size();
	}

	// Set column owner
	column->SetOwner(this);

	// Insert
	ColumnInfo* columnInfo = m_Columns.emplace(m_Columns.begin() + position, std::make_unique<ColumnInfo>(column))->get();
	OnColumnsCountChanged();
}
void KxDataViewCtrl::EnsureVisible(size_t row, size_t column)
{
	if (row > m_ClientArea->GetRowCount())
	{
		row = m_ClientArea->GetRowCount();
	}

	size_t first = m_ClientArea->GetFirstVisibleRow();
	size_t last = m_ClientArea->GetLastVisibleRow();
	if (row < first)
	{
		m_ClientArea->ScrollTo(row, column);
	}
	else if (row > last)
	{
		m_ClientArea->ScrollTo((intptr_t)row - (intptr_t)last + (intptr_t)first, column);
	}
	else
	{
		m_ClientArea->ScrollTo(first, column);
	}
}

KxDataViewItem KxDataViewCtrl::GetItemByRow(size_t row) const
{
	return m_ClientArea->GetItemByRow(row);
}
size_t KxDataViewCtrl::GetRowByItem(const KxDataViewItem & item) const
{
	return m_ClientArea->GetRowByItem(item);
}

void KxDataViewCtrl::UseColumnForSorting(size_t index)
{
	m_ColumnsSortingIndexes.push_back(index);
}
void KxDataViewCtrl::DontUseColumnForSorting(size_t index)
{
	for (auto it = m_ColumnsSortingIndexes.begin(); it < m_ColumnsSortingIndexes.end(); ++it)
	{
		if (*it == index)
		{
			m_ColumnsSortingIndexes.erase(it);
			return;
		}
	}

	wxFAIL_MSG("Column is not used for sorting");
}
bool KxDataViewCtrl::IsColumnSorted(size_t index) const
{
	return std::find(m_ColumnsSortingIndexes.begin(), m_ColumnsSortingIndexes.end(), index) != m_ColumnsSortingIndexes.end();
}

void KxDataViewCtrl::ResetAllSortColumns()
{
	// Must make copy, because unsorting will remove it from original vector
	auto copy = m_ColumnsSortingIndexes;
	for (auto& index: copy)
	{
		GetColumn(index)->ResetSorting();
	}

	wxASSERT(m_ColumnsSortingIndexes.empty());
}
void KxDataViewCtrl::OnInternalIdle()
{
	wxControl::OnInternalIdle();

	if (m_ColumnsDirty)
	{
		UpdateColWidths();
	}
}
void KxDataViewCtrl::DoEnableSystemTheme(bool enable, wxWindow* window)
{
	wxSystemThemedControlBase::DoEnableSystemTheme(enable, window);
	wxSystemThemedControlBase::DoEnableSystemTheme(enable, m_ClientArea);
	if (m_HeaderArea)
	{
		wxSystemThemedControlBase::DoEnableSystemTheme(enable, m_HeaderArea);
	}

	m_UsingSystemTheme = enable;
}

void KxDataViewCtrl::OnSize(wxSizeEvent& event)
{
	if (m_ClientArea && GetColumnCount() != 0)
	{
		m_ClientArea->UpdateColumnSizes();
	}

	// We need to override OnSize so that our scrolled
	// window a) does call Layout() to use sizers for
	// positioning the controls but b) does not query
	// the sizer for their size and use that for setting
	// the scrollable area as set that ourselves by
	// calling SetScrollbar() further down.

	Layout();
	AdjustScrollbars();

	// We must redraw the headers if their height changed. Normally this
	// shouldn't happen as the control shouldn't let itself be resized beneath
	// its minimal height but avoid the display artifacts that appear if it
	// does happen, e.g. because there is really not enough vertical space.
	if (m_HeaderArea && m_HeaderArea->GetSize().y <= m_HeaderArea->GetBestSize().y)
	{
		m_HeaderArea->Refresh();
	}
}
void KxDataViewCtrl::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	if (m_BorderColor.IsOk())
	{
		dc.SetPen(m_BorderColor);
		dc.SetBrush(m_BorderColor);
		dc.DrawRectangle(GetClientSize());
	}
}

wxSize KxDataViewCtrl::GetSizeAvailableForScrollTarget(const wxSize& size)
{
	wxSize newSize = size;
	if (m_HeaderArea)
	{
		newSize.y -= m_HeaderArea->GetSize().y;
	}
	return newSize;
}

bool KxDataViewCtrl::Create(wxWindow*parent,
							wxWindowID id,
							long style,
							const wxValidator& validator
)
{
	if (wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style|wxScrolledWindowStyle, validator, GetClassInfo()->GetClassName()))
	{
		m_ClientArea = new KxDataViewMainWindow(this, wxID_NONE);
		m_Indent = wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y);

		// We use the cursor keys for moving the selection, not scrolling, so call
		// this method to ensure wxScrollHelperEvtHandler doesn't catch all
		// keyboard events forwarded to us from 'KxDataViewMainWindow'.
		DisableKeyboardScrolling();

		if (!HasFlag(KxDV_NO_HEADER))
		{
			m_HeaderArea = new KxDataViewHeaderCtrl(this);
		}
		SetTargetWindow(m_ClientArea);

		m_Sizer = new wxBoxSizer(wxVERTICAL);
		if (m_HeaderArea)
		{
			m_HeaderAreaSI = m_Sizer->Add(m_HeaderArea, 0, wxEXPAND|wxTOP|wxLEFT|wxRIGHT);
		}
		m_ClientAreaSI = m_Sizer->Add(m_ClientArea, 1, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT);
		SetSizer(m_Sizer);

		EnableSystemTheme();
		Bind(wxEVT_SIZE, &KxDataViewCtrl::OnSize, this);
		Bind(wxEVT_PAINT, &KxDataViewCtrl::OnPaint, this);
		return true;
	}
	return false;
}
KxDataViewCtrl::~KxDataViewCtrl()
{
	if (m_Model)
	{
		m_Model->DecRef();
		m_Model = nullptr;
	}
}

bool KxDataViewCtrl::AssociateModel(KxDataViewModel* model)
{
	// Discard old model, if any.
	if (m_Model)
	{
		m_Model->RemoveNotifier(m_Notifier);
		m_Model->DecRef();
		m_Model = nullptr;
	}

	// Add our own reference and notifier to the new model
	m_Notifier = nullptr;
	if (model)
	{
		m_Notifier = new KxDataViewModelGenerciNotifier(m_ClientArea);

		m_Model = model;
		m_Model->IncRef();
		m_Model->AddNotifier(m_Notifier);
	}

	// Rebuild tree
	m_ClientArea->DestroyTree();
	if (m_Model)
	{
		m_ClientArea->BuildTree(m_Model);
	}
	m_ClientArea->UpdateDisplay();
	return true;
}
bool KxDataViewCtrl::AssignModel(KxDataViewModel* model)
{
	if (AssociateModel(model))
	{
		while (m_Model->GetRefCount() > 1)
		{
			m_Model->DecRef();
		}
		return true;
	}
	return false;
}

/* Column management */
KxDataViewRenderer* KxDataViewCtrl::AppendColumn(KxDataViewColumn* column)
{
	DoInsertColumn(column, GetColumnCount());
	return column->GetRenderer();
}
KxDataViewRenderer* KxDataViewCtrl::PrependColumn(KxDataViewColumn* column)
{
	DoInsertColumn(column, 0);
	return column->GetRenderer();
}
KxDataViewRenderer* KxDataViewCtrl::InsertColumn(size_t pos, KxDataViewColumn* column)
{
	DoInsertColumn(column, pos);
	return column->GetRenderer();
}

size_t KxDataViewCtrl::GetVisibleColumnCount() const
{
	size_t count = 0;
	for (size_t i = 0; i < GetColumnCount(); i++)
	{
		if (GetColumn(i)->IsVisible())
		{
			count++;
		}
	}
	return count;
}

KxDataViewColumn* KxDataViewCtrl::GetColumn(size_t position) const
{
	if (position < m_Columns.size())
	{
		return m_Columns[position]->GetColumn();
	}
	return nullptr;
}
size_t KxDataViewCtrl::GetColumnPosition(const KxDataViewColumn* column) const
{
	size_t count = GetColumnCount();
	for (size_t i = 0; i < count; i++)
	{
		if (GetColumnAt(i) == column)
		{
			return i;
		}
	}
	return INVALID_ROW;
}

bool KxDataViewCtrl::DeleteColumn(KxDataViewColumn *column)
{
	auto it = GetColumnIterator(column);
	if (it != m_Columns.end())
	{
		m_Columns.erase(it);

		if (m_ClientArea->GetCurrentColumn() == column)
		{
			m_ClientArea->ClearCurrentColumn();
		}
		OnColumnsCountChanged();
	}
	return false;
}
bool KxDataViewCtrl::ClearColumns()
{
	SetExpanderColumn(nullptr);
	m_Columns.clear();
	m_ColumnsSortingIndexes.clear();
	m_ClientArea->ClearCurrentColumn();
	OnColumnsCountChanged();

	return true;
}

KxDataViewColumn* KxDataViewCtrl::GetExpanderColumnOrFirstOne()
{
	if (!m_ExpanderColumn)
	{
		// TODO-RTL: Last column for RTL support
		m_ExpanderColumn = GetColumnAt(0);
		SetExpanderColumn(m_ExpanderColumn);
	}
	return m_ExpanderColumn;
}

KxDataViewColumn* KxDataViewCtrl::GetSortingColumn() const
{
	return !m_ColumnsSortingIndexes.empty() ? GetColumn(m_ColumnsSortingIndexes.front()) : nullptr;
}
KxDataViewColumn::Vector KxDataViewCtrl::GetSortingColumns() const
{
	KxDataViewColumn::Vector columns;
	for (const auto& index: m_ColumnsSortingIndexes)
	{
		if (KxDataViewColumn* column = GetColumn(index))
		{
			columns.push_back(column);
		}
	}
	return columns;
}

bool KxDataViewCtrl::AllowMultiColumnSort(bool allow)
{
	if (m_AllowMultiColumnSort == allow)
	{
		return true;
	}
	m_AllowMultiColumnSort = allow;

	// If disabling, must disable any multiple sort that are active
	if (!allow)
	{
		ResetAllSortColumns();

		if (KxDataViewModel* model = GetModel())
		{
			model->Resort();
		}
	}
	return true;
}
void KxDataViewCtrl::ToggleSortByColumn(size_t position)
{
	m_HeaderArea->ToggleSortByColumn(position);
}

KxDataViewColumn* KxDataViewCtrl::GetCurrentColumn() const
{
	return m_ClientArea->GetCurrentColumn();
}

KxDataViewItem KxDataViewCtrl::GetCurrentItem() const
{
	return HasFlag(KxDV_MULTIPLE_SELECTION) ? DoGetCurrentItem() : GetSelection();
}
void KxDataViewCtrl::SetCurrentItem(const KxDataViewItem& item)
{
	wxCHECK_RET(item.IsOK(), "Can't make current an invalid item.");

	if (HasFlag(KxDV_MULTIPLE_SELECTION))
	{
		DoSetCurrentItem(item);
	}
	else
	{
		Select(item);
	}
}

int KxDataViewCtrl::GetSelectedItemsCount() const
{
	return m_ClientArea->GetSelections().GetSelectedCount();
}
KxDataViewItem KxDataViewCtrl::GetSelection() const
{
	const wxSelectionStore& selections = m_ClientArea->GetSelections();
	if (selections.GetSelectedCount() == 1)
	{
		wxSelectionStore::IterationState cookie = 0;
		return m_ClientArea->GetItemByRow(selections.GetFirstSelectedItem(cookie));
	}
	return KxDataViewItem();
}
size_t KxDataViewCtrl::GetSelections(KxDataViewItem::Vector& sel) const
{
	sel.clear();
	const wxSelectionStore& selections = m_ClientArea->GetSelections();

	wxSelectionStore::IterationState cookie;
	for (auto row = selections.GetFirstSelectedItem(cookie); row != wxSelectionStore::NO_SELECTION; row = selections.GetNextSelectedItem(cookie))
	{
		KxDataViewItem item = m_ClientArea->GetItemByRow(row);
		if (item.IsOK())
		{
			sel.push_back(item);
		}
	}
	return sel.size();
}
void KxDataViewCtrl::SetSelections(const KxDataViewItem::Vector& sel)
{
	m_ClientArea->ClearSelection();
	KxDataViewItem last_parent;

	for (size_t i = 0; i < sel.size(); i++)
	{
		KxDataViewItem item = sel[i];
		KxDataViewItem parent = m_Model->GetParent(item);
		if (parent)
		{
			if (parent != last_parent)
			{
				ExpandAncestors(item);
			}
		}

		last_parent = parent;
		size_t row = m_ClientArea->GetRowByItem(item);
		if (row != INVALID_ROW)
		{
			m_ClientArea->SelectRow(row, true);
		}
	}
}
void KxDataViewCtrl::Select(const KxDataViewItem& item)
{
	ExpandAncestors(item);

	size_t row = m_ClientArea->GetRowByItem(item);
	if (row != INVALID_ROW)
	{
		// Unselect all rows before select another in the single select mode
		if (m_ClientArea->IsSingleSelection())
		{
			m_ClientArea->UnselectAllRows();
		}
		m_ClientArea->SelectRow(row, true);

		// Also set focus to the selected item
		m_ClientArea->ChangeCurrentRow(row);
	}
}
void KxDataViewCtrl::Unselect(const KxDataViewItem& item)
{
	const size_t row = m_ClientArea->GetRowByItem(item);
	if (row != INVALID_ROW)
	{
		m_ClientArea->SelectRow(row, false);
	}
}
bool KxDataViewCtrl::IsSelected(const KxDataViewItem& item) const
{
	const size_t row = m_ClientArea->GetRowByItem(item);
	if (row != INVALID_ROW)
	{
		return m_ClientArea->IsRowSelected(row);
	}
	return false;
}

KxDataViewItem KxDataViewCtrl::GetHotTrackedItem() const
{
	return m_ClientArea->GetHotTrackItem();
}
KxDataViewColumn* KxDataViewCtrl::GetHotTrackedColumn() const
{
	return m_ClientArea->GetHotTrackColumn();
}

void KxDataViewCtrl::GenerateSelectionEvent(const KxDataViewItem& item, const KxDataViewColumn* column)
{
	m_ClientArea->SendSelectionChangedEvent(item, const_cast<KxDataViewColumn*>(column));
}

void KxDataViewCtrl::SelectAll()
{
	m_ClientArea->SelectAllRows();
}
void KxDataViewCtrl::UnselectAll()
{
	m_ClientArea->UnselectAllRows();
}

void KxDataViewCtrl::Expand(const KxDataViewItem& item)
{
	ExpandAncestors(item);

	size_t row = m_ClientArea->GetRowByItem(item);
	if (row != -1)
	{
		m_ClientArea->Expand(row);
	}
}
void KxDataViewCtrl::ExpandAncestors(const KxDataViewItem& item)
{
	if (m_Model && item.IsOK())
	{
		KxDataViewItem::Vector parentChain;

		// At first we get all the parents of the selected item
		KxDataViewItem parent = m_Model->GetParent(item);
		while (parent.IsOK())
		{
			parentChain.push_back(parent);
			parent = m_Model->GetParent(parent);
		}

		// Then we expand the parents, starting at the root
		while (!parentChain.empty())
		{
			Expand(parentChain.back());
			parentChain.pop_back();
		}
	}
}
void KxDataViewCtrl::Collapse(const KxDataViewItem& item)
{
	size_t row = m_ClientArea->GetRowByItem(item);
	if (row != -1)
	{
		m_ClientArea->Collapse(row);
	}
}
bool KxDataViewCtrl::IsExpanded(const KxDataViewItem& item) const
{
	size_t row = m_ClientArea->GetRowByItem(item);
	if (row != -1)
	{
		return m_ClientArea->IsExpanded(row);
	}
	return false;
}

void KxDataViewCtrl::EnsureVisible(const KxDataViewItem& item, const KxDataViewColumn* column)
{
	ExpandAncestors(item);
	m_ClientArea->RecalculateDisplay();

	size_t row = m_ClientArea->GetRowByItem(item);
	if (row != INVALID_ROW)
	{
		if (column)
		{
			EnsureVisible(row, GetColumnIndex(column));
		}
		else
		{
			EnsureVisible(row, -1);
		}
	}
}
void KxDataViewCtrl::HitTest(const wxPoint& point, KxDataViewItem& item, KxDataViewColumn*& column) const
{
	m_ClientArea->HitTest(point, item, column);
}
wxRect KxDataViewCtrl::GetItemRect(const KxDataViewItem& item, const KxDataViewColumn* column) const
{
	return m_ClientArea->GetItemRect(item, column);
}
wxRect KxDataViewCtrl::GetAdjustedItemRect(const KxDataViewItem& item, const KxDataViewColumn* column) const
{
	wxRect rect = GetItemRect(item, column);
	if (HasHeaderCtrl())
	{
		rect.SetTop(rect.GetTop() + GetHeaderCtrl()->GetSize().GetHeight());
	}
	return rect;
}
wxPoint KxDataViewCtrl::GetDropdownMenuPosition(const KxDataViewItem& item, const KxDataViewColumn* column) const
{
	return GetAdjustedItemRect(item, column).GetLeftBottom() + FromDIP(wxPoint(0, 1));
}

int KxDataViewCtrl::GetUniformRowHeight() const
{
	return m_ClientArea->GetUniformRowHeight();
}
void KxDataViewCtrl::SetUniformRowHeight(int rowHeight)
{
	m_ClientArea->SetUniformRowHeight(rowHeight);
}
int KxDataViewCtrl::GetDefaultRowHeight(KxDataViewDefaultRowHeightType type) const
{
	return m_ClientArea->GetDefaultRowHeight(type);
}

bool KxDataViewCtrl::EditItem(const KxDataViewItem& item, const KxDataViewColumn* column)
{
	if (item.IsOK() && column)
	{
		return m_ClientArea->BeginEdit(item, column);
	}
	return false;
}

/* Drag and drop */
bool KxDataViewCtrl::EnableDragSource(const wxDataFormat& format)
{
	return m_ClientArea->EnableDragSource(format);
}
bool KxDataViewCtrl::EnableDropTarget(const wxDataFormat& format)
{
	return m_ClientArea->EnableDropTarget(format);
}

void KxDataViewCtrl::SetFocus()
{
	if (m_ClientArea)
	{
		m_ClientArea->SetFocus();
	}
}
bool KxDataViewCtrl::SetFont(const wxFont& font)
{
	if (!wxControl::SetFont(font))
	{
		return false;
	}

	if (m_HeaderArea)
	{
		m_HeaderArea->SetFont(font);
	}
	if (m_ClientArea)
	{
		m_ClientArea->SetFont(font);
		m_ClientArea->SetUniformRowHeight(m_ClientArea->GetDefaultRowHeight());
	}

	if (m_HeaderArea || m_ClientArea)
	{
		InvalidateColBestWidths();
		Layout();
	}
	return true;
}

/* Window */
wxHeaderCtrl* KxDataViewCtrl::GetHeaderCtrl()
{
	return m_HeaderArea;
}
const wxHeaderCtrl* KxDataViewCtrl::GetHeaderCtrl() const
{
	return m_HeaderArea;
}

bool KxDataViewCtrl::CreateColumnSelectionMenu(KxMenu& menu)
{
	size_t count = GetColumnCount();
	for (size_t i = 0; i < count; i++)
	{
		KxDataViewColumn* column = GetColumn(i);
		wxString title = column->GetTitle();
		if (title.IsEmpty())
		{
			title << '<' << i + 1 << '>';
		}

		KxMenuItem* menuItem = menu.Add(new KxMenuItem(title, wxEmptyString, wxITEM_CHECK));
		menuItem->Check(column->IsShown());
		menuItem->SetClientData(column);
	}
	return menu.GetMenuItemCount() != 0;
}
KxDataViewColumn* KxDataViewCtrl::OnColumnSelectionMenu(KxMenu& menu)
{
	wxWindowID retID = menu.Show(this);
	if (retID != KxID_NONE)
	{
		KxMenuItem* menuItem = menu.FindItem(retID);
		KxDataViewColumn* column = static_cast<KxDataViewColumn*>(menuItem->GetClientData());
		column->SetHidden(!menuItem->IsChecked());

		size_t index = GetColumnIndex(column);
		if (column->IsShown() && column->GetWidth() == 0)
		{
			column->SetWidth(m_ClientArea->GetBestColumnWidth(index));
		}

		OnColumnChange(index);
		return column;
	}
	return nullptr;
}

/* Control visuals */
wxBorder KxDataViewCtrl::GetDefaultBorder() const
{
	return wxBORDER_THEME;
}
void KxDataViewCtrl::SetBorderColor(const KxColor& color, int size)
{
	m_BorderColor = color;
	int borderSize = m_BorderColor.IsOk() ? FromDIP(size) : 0;

	m_ClientAreaSI->SetBorder(borderSize);
	if (m_HeaderAreaSI)
	{
		m_HeaderAreaSI->SetBorder(borderSize);
	}
	Refresh();
}

size_t KxDataViewCtrl::GetBestColumnWidth(size_t index) const
{
	return m_ClientArea->GetBestColumnWidth(index);
}

/* Utility functions, not part of the API */
void KxDataViewCtrl::ColumnMoved(KxDataViewColumn* column, size_t new_pos)
{
	// Do *not* reorder 'm_Columns' elements here, they should always be in the order in which columns
	// were added, we only display the columns in different order.
	UpdateColumnsOrderArray();

	OnColumnChange(GetColumnPosition(column));
	OnColumnChange(new_pos);
}
void KxDataViewCtrl::OnColumnChange(size_t index)
{
	if (m_HeaderArea)
	{
		m_HeaderArea->UpdateColumn(index);
		UpdateColumnsOrderArray();
	}
	m_ClientArea->UpdateDisplay();
}
void KxDataViewCtrl::OnColumnsCountChanged()
{
	if (m_HeaderArea)
	{
		m_HeaderArea->SetColumnCount(GetColumnCount());
		UpdateColumnsOrderArray();
	}
	m_ClientArea->OnColumnsCountChanged();
}

size_t KxDataViewCtrl::GetColumnIndex(const KxDataViewColumn* column) const
{
	auto it = GetColumnIterator(column);
	if (it != m_Columns.end())
	{
		return std::distance(m_Columns.begin(), it);
	}
	return INVALID_ROW;
}
size_t KxDataViewCtrl::GetColumnIndexByID(int columnID) const
{
	auto it = std::find_if(m_Columns.begin(), m_Columns.end(), [columnID](const auto& value)
	{
		return value->GetColumn()->GetID() == columnID;
	});

	if (it != m_Columns.end())
	{
		return std::distance(m_Columns.begin(), it);
	}
	return INVALID_ROW;
}

KxDataViewColumn* KxDataViewCtrl::GetColumnAt(size_t position) const
{
	// Columns can't be reordered if there is no header window which allows to do this.
	if (HasHeaderCtrl())
	{
		// Update needed
		if (m_ColumnsOrder.size() != GetColumnCount())
		{
			const_cast<KxDataViewCtrl*>(this)->UpdateColumnsOrderArray();
		}

		if (position < m_ColumnsOrder.size())
		{
			return GetColumn(m_ColumnsOrder[position]);
		}
	}
	else
	{
		return GetColumn(position);
	}
	return nullptr;
}
KxDataViewColumn* KxDataViewCtrl::GetColumnAtVisible(size_t position) const
{
	size_t visibleIndex = 0;
	auto TestColumn = [this, &visibleIndex](size_t i, KxDataViewColumn*& column)
	{
		column = m_Columns[i]->GetColumn();
		if (column->IsVisible())
		{
			visibleIndex++;
		}
		return visibleIndex != 0 ? visibleIndex - 1 : INVALID_COLUMN;
	};

	if (HasHeaderCtrl())
	{
		// Update needed
		if (m_ColumnsOrder.size() != GetColumnCount())
		{
			const_cast<KxDataViewCtrl*>(this)->UpdateColumnsOrderArray();
		}

		for (size_t i: m_ColumnsOrder)
		{
			KxDataViewColumn* column = nullptr;
			if (TestColumn(i, column) == position)
			{
				return column;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_Columns.size(); i++)
		{
			KxDataViewColumn* column = nullptr;
			if (TestColumn(i, column) == position)
			{
				return column;
			}
		}
	}
	return nullptr;
}
