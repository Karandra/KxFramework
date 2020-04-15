#include "KxStdAfx.h"
#include "KxFramework/KxListView.h"
#include "KxFramework/KxIncludeWindows.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxListView, wxListView);

void KxListView::UpdateTileViewItem(wxListEvent& event)
{
	if (m_InTileView)
	{
		HWND hWnd = GetHandle();
		LVTILEINFO itemInfo = {0};
		itemInfo.cbSize = sizeof(itemInfo);
		itemInfo.cColumns = GetColumnCount();
		itemInfo.iItem = event.GetIndex();
		PUINT subItems = new UINT[itemInfo.cColumns];
		PINT format = new INT[itemInfo.cColumns];

		for (size_t c = 0; c < itemInfo.cColumns; c++)
		{
			subItems[c] = c+1;
			format[c] = LVCFMT_LINE_BREAK|LVCFMT_FILL|LVCFMT_WRAP;
		}
		itemInfo.puColumns = subItems;
		itemInfo.piColFmt = format;
		::SendMessageW(hWnd, LVM_SETTILEINFO, 0, (LPARAM)&itemInfo);

		delete[] subItems;
		delete[] format;
	}
}
void KxListView::UpdateTileView()
{
	if (m_InTileView)
	{
		SetSingleStyle(KxLV_LIST, false);
		SetSingleStyle(KxLV_REPORT, false);
		SetSingleStyle(KxLV_ICON, false);
		SetSingleStyle(KxLV_SMALL_ICON, false);

		// Configure tile view
		HWND hWnd = GetHandle();
		LVTILEVIEWINFO tileInfo = {0};
		tileInfo.cbSize = sizeof(tileInfo);
		tileInfo.dwMask = LVTVIM_TILESIZE|LVTVIM_COLUMNS;
		tileInfo.cLines = GetColumnCount();

		SIZE size = {0};
		if (m_TileViewAutoSize || (m_TileViewWidth == -1 && m_TileViewHeight == -1))
		{
			tileInfo.dwFlags = LVTVIF_AUTOSIZE;
		}
		else
		{
			if (m_TileViewWidth != -1)
			{
				tileInfo.dwFlags = tileInfo.dwFlags|LVTVIF_FIXEDWIDTH;
				size.cx = m_TileViewWidth;
			}

			if (m_TileViewHeight != -1)
			{
				tileInfo.dwFlags = tileInfo.dwFlags|LVTVIF_FIXEDHEIGHT;
				size.cy = m_TileViewHeight;
			}
			tileInfo.sizeTile = size;
		}
		SendMessageW(hWnd, LVM_SETTILEVIEWINFO, LV_VIEW_TILE, (LPARAM)&tileInfo);

		// Configure items for tile view
		LVTILEINFO itemInfo = {0};
		itemInfo.cbSize = sizeof(itemInfo);
		itemInfo.cColumns = tileInfo.cLines;
		PUINT subItems = new UINT[itemInfo.cColumns];
		int* format = new int[itemInfo.cColumns];

		for (int i = 0; i < GetItemCount(); i++)
		{
			itemInfo.iItem = i;
			for (size_t c = 0; c < itemInfo.cColumns; c++)
			{
				subItems[c] = c+1;
				format[c] = LVCFMT_LINE_BREAK|LVCFMT_FILL|LVCFMT_WRAP;
			}
			itemInfo.puColumns = subItems;
			itemInfo.piColFmt = format;
			SendMessageW(hWnd, LVM_SETTILEINFO, 0, (LPARAM)&itemInfo);
		}
		delete[] subItems;
		delete[] format;

		SendMessageW(hWnd, LVM_SETVIEW, LV_VIEW_TILE, 0);
	}
}
void KxListView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	dc.Clear();
	event.Skip();
}

bool KxListView::Create(wxWindow* parent,
						wxWindowID winid,
						long style
)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	if (wxListView::Create(parent, winid, wxDefaultPosition, wxDefaultSize, style))
	{
		m_InTileView = style & KxLV_TILE;
		ListView_SetExtendedListViewStyleEx(GetHandle(), LVS_EX_COLUMNSNAPPOINTS, LVS_EX_COLUMNSNAPPOINTS);

		Bind(wxEVT_PAINT, &KxListView::OnPaint, this);
		return true;
	}
	return false;
}
KxListView::~KxListView()
{
}

int KxListView::GetViewMode()
{
	if (SendMessageW(GetHandle(), LVM_GETVIEW, 0, 0) == LV_VIEW_TILE)
	{
		return KxLV_TILE;
	}
	else if (HasFlag(KxLV_LIST))
	{
		return KxLV_LIST;
	}
	else if (HasFlag(KxLV_ICON))
	{
		return KxLV_ICON;
	}
	else if (HasFlag(KxLV_SMALL_ICON))
	{
		return KxLV_SMALL_ICON;
	}
	else
	{
		return KxLV_REPORT;
	}
}
void KxListView::SetViewMode(int viewMode)
{
	m_InTileView = viewMode & KxLV_TILE;
	if (viewMode == KxLV_TILE)
	{
		UpdateTileView();
		Bind(wxEVT_LIST_INSERT_ITEM, &KxListView::UpdateTileViewItem, this);
	}
	else
	{
		SetSingleStyle(viewMode, true);
		Unbind(wxEVT_LIST_INSERT_ITEM, &KxListView::UpdateTileViewItem, this);
	}
}
bool KxListView::IsMultiSelect()
{
	return !HasFlag(KxLV_SINGLE_SEL);
}
void KxListView::SetMultiSelect(bool value)
{
	SetSingleStyle(KxLV_SINGLE_SEL, !value);
}

// Columns
KxListView::SortArrow KxListView::GetSortArrow(int index) const
{
	HDITEMW item = {0};
	HWND headerWnd = ListView_GetHeader(GetHandle());
	if (headerWnd)
	{
		item.mask = HDI_FORMAT;
		if (Header_SetItem(headerWnd, index, &item))
		{
			if (item.fmt & HDF_SORTUP)
			{
				return ARROW_UP;
			}
			else if (item.fmt & HDF_SORTDOWN)
			{
				return ARROW_DOWN;
			}
			else
			{
				return ARROW_NONE;
			}
		}
	}
	return ARROW_NONE;
}
void KxListView::SetSortArrow(int index, SortArrow mode)
{
	HDITEMW item = {0};
	HWND headerWnd = ListView_GetHeader(GetHandle());
	if (headerWnd)
	{
		item.mask = HDI_FORMAT;
		if (Header_SetItem(headerWnd, index, &item))
		{
			switch (mode)
			{
				case ARROW_UP:
				{
					item.fmt = (item.fmt & ~HDF_SORTDOWN) | HDF_SORTUP;
					break;
				}
				case ARROW_DOWN:
				{
					item.fmt = (item.fmt & ~HDF_SORTUP) | HDF_SORTDOWN;
					break;
				}
				default:
				{
					item.fmt = item.fmt & ~(HDF_SORTDOWN | HDF_SORTUP);
				}
			};
			Header_SetItem(headerWnd, index, &item);
		}
	}
}

int KxListView::InsertColumn(const wxString& label, int width, size_t index, int imageID, wxListColumnFormat alignment)
{
	wxListItem item;
	item.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE|wxLIST_MASK_WIDTH|wxLIST_MASK_FORMAT);
	item.SetText(label);
	item.SetImage(imageID);
	item.SetWidth(width);
	item.SetAlign(alignment);
	item.SetColumn(index);
	item.SetImage(imageID);

	return wxListView::InsertColumn(index, item);
}
int KxListView::AddColumn(const wxString& label, int width, int imageID, wxListColumnFormat alignment)
{
	return InsertColumn(label, width, GetColumnCount() + 1, imageID, alignment);
}
void KxListView::RemoveColumn(int i)
{
	wxListView::DeleteColumn(i);
}
void KxListView::ClearColumns()
{
	wxListView::ClearAll();
}
void KxListView::SelectColumn(int i)
{
	SendMessageW(GetHandle(), LVM_SETSELECTEDCOLUMN, i, 0);
}
wxString KxListView::GetColumnLabel(int i) const
{
	wxListItem column;
	column.SetMask(wxLIST_MASK_TEXT);
	column.SetColumn(i);
	if (GetColumn(i, column))
	{
		return column.GetText();
	}
	return {};
}
void KxListView::SetColumnLabel(int i, const wxString& label)
{
	wxListItem column;
	if (GetColumn(i, column))
	{
		column.SetMask(wxLIST_MASK_TEXT);
		column.SetText(label);
		SetColumn(i, column);
	}
}
int KxListView::GetColumnWidth(int i) const
{
	wxListItem column;
	column.SetMask(wxLIST_MASK_WIDTH);
	column.SetColumn(i);
	if (GetColumn(i, column))
	{
		return column.GetWidth();
	}
	return KxLV_INVALID_WIDTH;
}
bool KxListView::SetColumnWidth(int i, int width)
{
	wxListItem column;
	if (GetColumn(i, column))
	{
		column.SetMask(wxLIST_MASK_WIDTH);
		column.SetWidth(width);
		SetColumn(i, column);
		return true;
	}
	return false;
}
int KxListView::GetColumnImage(int i) const
{
	wxListItem column;
	column.SetMask(wxLIST_MASK_IMAGE);
	column.SetColumn(i);
	if (GetColumn(i, column))
	{
		return column.GetWidth();
	}
	return wxWithImages::NO_IMAGE;
}
void KxListView::SetColumnImage(int i, int imageID)
{
	wxListItem column;
	if (GetColumn(i, column))
	{
		column.SetMask(wxLIST_MASK_IMAGE);
		column.SetWidth(imageID);
		SetColumn(i, column);
	}
}
wxListColumnFormat KxListView::GetColumnAlignment(int i) const
{
	wxListItem column;
	column.SetMask(wxLIST_MASK_FORMAT);
	column.SetColumn(i);
	if (GetColumn(i, column))
	{
		return column.GetAlign();
	}
	return wxLIST_FORMAT_LEFT;
}
void KxListView::SetColumnAlignment(int i, wxListColumnFormat imageID)
{
	wxListItem column;
	if (GetColumn(i, column))
	{
		column.SetMask(wxLIST_MASK_FORMAT);
		column.SetAlign(imageID);
		SetColumn(i, column);
	}
}

// Items
void KxListView::ClearItems()
{
	DeleteAllItems();
}
void KxListView::RemoveItem(int i)
{
	DeleteItem(i);
}
int KxListView::GetSelection() const
{
	return GetFirstSelected();
}
KxIntVector KxListView::GetSelections() const
{
	KxIntVector list;
	list.reserve(GetSelectedItemCount());
	for (int i = GetFirstSelected(); i != -1; i = GetNextSelected(i))
	{
		list.push_back(i);
	}
	return list;
}
KxIntVector KxListView::GetCheckedItems() const
{
	KxIntVector list;
	if (HasCheckBoxes())
	{
		int count = GetItemCount();
		list.reserve(count);
		for (int i = 0; i < count; i++)
		{
			if (IsItemChecked(i))
			{
				list.push_back(i);
			}
		}
	}
	return list;
}
int KxListView::InsertItem(const wxString& label, size_t index, int imageID)
{
	wxListItem item;
	item.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
	item.SetId(index);
	item.SetColumn(0);
	item.SetImage(imageID);

	int newIndex = wxListView::InsertItem(index, imageID);
	if (newIndex != -1)
	{
		wxListView::SetItem(newIndex, 0, label, imageID);
	}
	return newIndex;
}
int KxListView::InsertItem(const KxStringVector& labels, size_t index, int imageID)
{
	wxListItem item;
	item.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
	item.SetId(index);
	item.SetColumn(0);
	item.SetImage(imageID);

	int newIndex = wxListView::InsertItem(index, imageID);
	if (newIndex != -1)
	{
		int columnIndex = item.GetColumn();
		for (size_t i = 0; i < std::min((size_t)GetColumnCount(), labels.size()); i++)
		{
			int nCurrentImage = -1;
			if ((size_t)columnIndex == i || columnIndex == -1)
			{
				nCurrentImage = imageID;
			}

			wxListView::SetItem(newIndex, i, labels[i], nCurrentImage);
		}
	}
	return newIndex;
}
int KxListView::AddItem(const wxString& label, int imageID)
{
	return InsertItem(label, GetItemCount() + 1, imageID);
}
int KxListView::AddItem(const KxStringVector& labels, int imageID)
{
	return InsertItem(labels, GetItemCount() + 1, imageID);
}
bool KxListView::IsItemSelected(int i) const
{
	return IsSelected(i);
}
void KxListView::FocusItem(int i)
{
	Focus(i);
}
void KxListView::SetAllItemSelected(bool b)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		Select(i, b);
	}
}
void KxListView::SelectItem(int i, bool b)
{
	Select(i, b);
}
void KxListView::SetAllItemChecked(bool b)
{
	wxEventBlocker block(this, b ? wxEVT_LIST_ITEM_CHECKED : wxEVT_LIST_ITEM_UNCHECKED);
	for (int i = 0; i < GetItemCount(); i++)
	{
		CheckItem(i, b);
	}
}
void KxListView::SetHotItem(int i)
{
	SendMessageW(GetHandle(), LVM_SETHOTITEM, i, 0);
}

wxString KxListView::GetItemLabel(int row, int columnIndex) const
{
	return GetItemText(row, columnIndex);
}
KxStringVector KxListView::GetItemLabels(int row) const
{
	KxStringVector labels;
	labels.reserve(GetColumnCount());
	for (int i = 0; i < GetColumnCount(); i++)
	{
		labels.push_back(GetItemText(row, i));
	}
	return labels;
}
void KxListView::SetItemLabel(const wxString& label, int row, int columnIndex)
{
	wxListItem item;
	item.SetColumn(columnIndex);
	item.SetMask(wxLIST_MASK_IMAGE);

	int nImage = -1;
	if (GetItem(item))
	{
		item.SetMask(wxLIST_MASK_IMAGE);
		nImage = item.GetImage();
	}
	SetItem(row, item.GetColumn(), label, nImage);
}
void KxListView::SetItemLabels(const KxStringVector& labels, int row)
{
	wxListItem item;
	int nImage = -1;
	for (size_t i = 0; i < std::min((size_t)GetColumnCount(), labels.size()); i++)
	{
		item.SetId(row);
		item.SetColumn(i);
		item.SetMask(wxLIST_MASK_IMAGE);
		if (GetItem(item))
		{
			item.SetMask(wxLIST_MASK_IMAGE);
			nImage = item.GetImage();
		}

		SetItem(row, i, labels[i], nImage);
	}
}
int KxListView::GetItemImage(int row, int columnIndex) const
{
	wxListItem item;
	item.SetMask(wxLIST_MASK_IMAGE);
	item.SetId(row);
	item.SetImage(-1);
	item.SetColumn(columnIndex);
	GetItem(item);

	return item.GetImage();
}
void KxListView::SetItemImage(int row, int columnIndex, int imageID)
{
	wxListItem item;
	item.SetId(row);
	if (GetItem(item))
	{
		if (columnIndex == 0)
		{
			SetItemImage(row, imageID, imageID);
		}
		else if (columnIndex != -1)
		{
			item.SetMask(wxLIST_MASK_IMAGE|wxLIST_MASK_TEXT);
			item.SetColumn(columnIndex);
			item.SetText(GetItemText(row, columnIndex));
			item.SetImage(imageID);
			SetItem(item);
		}
		else
		{
			item.SetImage(imageID);
			for (int i = 0; i < GetColumnCount(); i++)
			{
				SetItem(row, i, GetItemText(row, i), imageID);
			}
		}
	}
}
void KxListView::SetItemChecked(int row, bool b)
{
	wxEventBlocker block(this, b ? wxEVT_LIST_ITEM_CHECKED : wxEVT_LIST_ITEM_UNCHECKED);
	CheckItem(row, b);
}
int KxListView::GetItemState(int row) const
{
	return wxListView::GetItemState(row, 0xFFFFFFFF);
}
void KxListView::SetItemState(int row, int state, bool set)
{
	wxListView::SetItemState(row, set ? state : 0, state);
}
wxFont KxListView::GetItemFont(int row, int columnIndex) const
{
	wxListItem item;
	item.SetId(row);
	item.SetColumn(columnIndex);
	if (GetItem(item))
	{
		return item.GetFont();
	}
	return wxNullFont;
}
void KxListView::SetItemFont(int row, int columnIndex, const wxFont& v)
{
	wxListItem item;
	item.SetId(row);
	item.SetId(columnIndex);
	if (GetItem(item))
	{
		item.SetFont(v);
		SetItem(item);
	}
}
