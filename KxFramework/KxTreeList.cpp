#include "KxStdAfx.h"
#include "KxFramework/KxTreeList.h"
#include "KxFramework/KxUtility.h"

#include <WindowsX.h>
#include "KxFramework/KxWinUndef.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxTreeList, wxTreeListCtrl);
wxDEFINE_EVENT(KxEVT_TREELIST_ITEM_ACTIVATED, wxTreeListEvent);
wxDEFINE_EVENT(KxEVT_TREELIST_SELECTION_CHANGED, wxTreeListEvent);

wxTreeListModelNode* KxTreeList::GetItemNode(const wxTreeListItem& item) const
{
	return (wxTreeListModelNode*)GetItemDV(item).GetID();
}
wxTreeListItem KxTreeList::DVItemToTL(const wxDataViewItem& item) const
{
	return wxTreeListItem((wxTreeListModelNode*)item.GetID());
}
void KxTreeList::InitHeaderWindow()
{
	m_HeaderCtrl = nullptr;//m_DataView->GetHeaderCtrl();
	if (m_HeaderCtrl)
	{
		if (m_Options & KxTL_FIX_FIRST_COLUMN)
		{
			m_HeaderCtrl->Bind(wxEVT_HEADER_END_RESIZE, &KxTreeList::OnHeaderResized, this);
		}
	}
}
void KxTreeList::SetDoubleClickExpandEnabled(bool value)
{
	if (value)
	{
		Bind(wxEVT_TREELIST_ITEM_ACTIVATED, &KxTreeList::EvtExpandOnDClick, this);
	}
	else
	{
		Unbind(wxEVT_TREELIST_ITEM_ACTIVATED, &KxTreeList::EvtExpandOnDClick, this);
	}
}

void KxTreeList::OnResize(wxSizeEvent& event)
{
	SetColumnWidth(0, m_FirstColumnWidth);
	event.Skip();
}
void KxTreeList::OnHeaderResized(wxHeaderCtrlEvent& event)
{
	if (event.GetColumn() == 0)
	{
		m_FirstColumnWidth = event.GetWidth();
	}
	event.Skip();
}

void KxTreeList::OnSelectDV(wxDataViewEvent& event)
{
	#if 0
	auto item = event.GetItem();
	size_t columnIndex = event.GetColumn();

	wxTreeListEvent treeListEvent(KxEVT_TREELIST_SELECTION_CHANGED, this, DVItemToTL(item));
	treeListEvent.SetEventObject(this);
	treeListEvent.m_column = columnIndex;
	HandleWindowEvent(treeListEvent);
	#endif
	event.Skip();
}
void KxTreeList::OnActivateDV(wxDataViewEvent& event)
{
	#if 0
	auto item = event.GetItem();
	size_t columnIndex = event.GetColumn();

	wxTreeListEvent treeListEvent(KxEVT_TREELIST_ITEM_ACTIVATED, this, DVItemToTL(item));
	treeListEvent.SetEventObject(this);
	treeListEvent.m_column = columnIndex;
	HandleWindowEvent(treeListEvent);
	#endif

	event.Skip();
}
void KxTreeList::EvtExpandOnDClick(wxTreeListEvent& event)
{
	KxTreeListItem item(*this, event.GetItem());
	item.SetExpanded(!item.IsExpanded());
	event.Skip();
}

wxDataViewRenderer* KxTreeList::GetColumnRendererDV(size_t index) const
{
	if (index != -1)
	{
		wxDataViewColumn* column = GetColumnDV(index);
		if (column != NULL)
		{
			return column->GetRenderer();
		}
	}
	return NULL;
}

bool KxTreeList::Create(wxWindow* parent,
						wxWindowID id,
						long style
)
{
	m_Options = (KxTreeListOptions)(style & KxTL_MASK);
	if (wxTreeListCtrl::Create(parent, id, wxDefaultPosition, wxDefaultSize, KxUtility::ModFlag(style, KxTL_MASK, false)))
	{
		wxWindowUpdateLocker lock(this);

		m_DataView = GetDataView();
		//m_DataView->SetAllowColumnsAutoSize(false);

		// Additional styles form DataView
		int styleDV = 0;
		if (m_Options & KxTL_VERT_RULES)
		{
			styleDV |= wxDV_VERT_RULES;
		}
		if (m_Options & KxTL_HORIZ_RULES)
		{
			styleDV |= wxDV_HORIZ_RULES;
		}
		if (styleDV != 0)
		{
			m_DataView->SetWindowStyle(m_DataView->GetWindowStyle()|styleDV);
		}

		SetDoubleBuffered(true);
		SetIndent(DefaultIndent);
		SetRowHeight(DefaultRowHeight);

		// Events
		InitHeaderWindow();
		SetDoubleClickExpandEnabled(m_Options & KxTL_DCLICK_EXPAND);
		if (m_Options & KxTL_FIX_FIRST_COLUMN)
		{
			Bind(wxEVT_SIZE, &KxTreeList::OnResize, this);
		}
		Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &KxTreeList::OnSelectDV, this);
		Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &KxTreeList::OnActivateDV, this);

		#if KxTreeList_HOTTARCK_ENABLED
		Bind(wxEVT_PAINT, &wxLTreeList::OnDrawItemHover, this);
		#endif

		return true;
	}
	return false;
}

wxDataViewItem KxTreeList::GetItemDV(const wxTreeListItem& item) const
{
	if (GetItemParent(item).IsOk())
	{
		return wxDataViewItem(item.GetID());
	}
	return wxDataViewItem();
}
wxDataViewColumn* KxTreeList::GetColumnDV(size_t index) const
{
	return m_DataView->GetColumn(index);
}

int KxTreeList::AddColumn(const wxString& label, int width, wxAlignment align, int flags)
{
	int columnIndex = wxTreeListCtrl::AppendColumn(label, width, align, flags);
	if (columnIndex != -1)
	{
		wxDataViewRenderer* renderer = GetColumnRendererDV(columnIndex);
		if (renderer)
		{
			renderer->EnableEllipsize(wxELLIPSIZE_END);
		}

		wxDataViewColumn* column = GetColumnDV(columnIndex);
		if (column)
		{
			column->SetMinWidth(WidthFor(label));
		}
	}
	if (columnIndex == 0)
	{
		m_FirstColumnWidth = GetColumnWidth(columnIndex);
	}
	return columnIndex;
}
void KxTreeList::SetColumnDisplayOrder(int oldIndex, int newIndex)
{
	if (m_HeaderCtrl && oldIndex != newIndex && newIndex >= 0 && (size_t)newIndex < GetColumnCount())
	{
		auto order = m_HeaderCtrl->GetColumnsOrder();

		order[oldIndex] = newIndex;
		order[newIndex] = oldIndex;
		
		m_HeaderCtrl->SetColumnsOrder(order);
	}
}
void KxTreeList::SetColumnWidth(unsigned col, int width)
{
	if (col == 0)
	{
		m_FirstColumnWidth = width;
	}
	wxTreeListCtrl::SetColumnWidth(col, width);
}

int KxTreeList::GetItemImage(const wxTreeListItem& item, int mode)
{
	auto node = GetItemNode(item);
	if (node)
	{
		if (mode == KxTL_IMAGE_EXPANDED)
		{
			//return node->m_imageOpened;
		}
		else if (mode == KxTL_IMAGE_COLLAPSED)
		{
			//return node->m_imageClosed;
		}
	}
	return NO_IMAGE;
}
void KxTreeList::SetItemImage(const wxTreeListItem& item, int nImage, int mode)
{
	if (mode == KxTL_IMAGE_COLLAPSED)
	{
		wxTreeListCtrl::SetItemImage(item, nImage, GetItemImage(item, KxTL_IMAGE_EXPANDED));
	}
	else
	{
		wxTreeListCtrl::SetItemImage(item, GetItemImage(item, KxTL_IMAGE_COLLAPSED), nImage);
	}
}
wxRect KxTreeList::GetItemRect(const wxDataViewItem& item, size_t columnIndex) const
{
	return m_DataView->GetItemRect(item, columnIndex != -1 ? GetColumnDV(columnIndex) : NULL);
}
wxRect KxTreeList::GetItemRect(const wxTreeListItem& item, size_t columnIndex) const
{
	return GetItemRect(GetItemDV(item), columnIndex);
}
wxRect KxTreeList::GetHeaderRect() const
{
	if (m_HeaderCtrl)
	{
		return m_HeaderCtrl->GetRect();
	}
	return wxRect(0, 0, 0, 0);
}
wxTreeListItem KxTreeList::GetPrevSibling(const wxTreeListItem& item) const
{
	wxTreeListItem prevItem;
	wxTreeListItem currentItem;
	for (currentItem = GetFirstChild(GetItemParent(item)); currentItem.IsOk(); currentItem = GetNextSibling(currentItem))
	{
		if (currentItem == item)
		{
			return prevItem;
		}
		prevItem = currentItem;
	}
	return wxTreeListItem(NULL);
}

KxTreeListItem KxTreeList::GetSelection() const
{
	if (HasFlag(wxTL_MULTIPLE))
	{
		wxTreeListItems items;
		wxTreeListCtrl::GetSelections(items);
		
		if (!items.empty())
		{
			return KxTreeListItem(*const_cast<KxTreeList*>(this), items[0]);
		}
		else
		{
			return KxTreeListItem(*const_cast<KxTreeList*>(this), wxTreeListItem());
		}
	}
	else
	{
		return KxTreeListItem(*const_cast<KxTreeList*>(this), wxTreeListCtrl::GetSelection());
	}
}
KxTreeListItems KxTreeList::GetSelections() const
{
	KxTreeListItems list;
	if (HasFlag(wxTL_MULTIPLE))
	{
		wxTreeListItems items;
		wxTreeListCtrl::GetSelections(items);

		list.reserve(items.size());
		for (const auto& v: items)
		{
			list.push_back(KxTreeListItem(*const_cast<KxTreeList*>(this), v));
		}
	}
	else
	{
		list.reserve(1);
		list.push_back(KxTreeListItem(*const_cast<KxTreeList*>(this), wxTreeListCtrl::GetSelection()));
	}
	return list;
}

//////////////////////////////////////////////////////////////////////////
void KxTreeListItem::OnCreate()
{
}

const KxTreeListItem KxTreeListItem::First = KxTreeListItem(wxTLI_FIRST);
const KxTreeListItem KxTreeListItem::Last = KxTreeListItem(wxTLI_LAST);
KxTreeListItem::KxTreeListItem(KxTreeList& control, const wxTreeListItem& v)
	:m_Control(&control), m_Item(v)
{
	OnCreate();
}
void KxTreeListItem::Create(KxTreeList& control, const wxTreeListItem& v)
{
	m_Control = &control;
	m_Item = v;
}

KxTreeListItem KxTreeListItem::Insert(const KxTreeListItem& previous, const wxString& label, wxClientData* data)
{
	return KxTreeListItem(*m_Control, m_Control->InsertItem(m_Item, previous, label, NO_IMAGE, NO_IMAGE, data));
}
KxTreeListItem KxTreeListItem::Insert(const KxTreeListItem& previous, const KxStringVector& labels, wxClientData* data)
{
	KxTreeListItem item = Insert(previous, wxEmptyString, data);
	item.SetLabels(labels);
	return item;
}

KxTreeListItem KxTreeListItem::GetFirstChild() const
{
	return KxTreeListItem(*m_Control, m_Control->GetFirstChild(m_Item));
}
KxTreeListItem KxTreeListItem::GetNthChild(size_t index) const
{
	KxTreeListItem item = GetFirstChild();
	if (item.IsOK())
	{
		if (index == 0)
		{
			return item;
		}
		else
		{
			for (size_t i = 1; i != index && item.IsOK(); i++)
			{
				item = item.GetNextSibling();
			}
		}
	}
	return item;
}
KxTreeListItem KxTreeListItem::GetNext() const
{
	return KxTreeListItem(*m_Control, m_Control->GetNextItem(m_Item));
}
KxTreeListItem KxTreeListItem::GetPrevSibling() const
{
	return KxTreeListItem(*m_Control, m_Control->GetPrevSibling(m_Item));
}
KxTreeListItem KxTreeListItem::GetNextSibling() const
{
	return KxTreeListItem(*m_Control, m_Control->GetNextSibling(m_Item));
}
KxTreeListItem KxTreeListItem::GetParent() const
{
	return KxTreeListItem(*m_Control, m_Control->GetItemParent(m_Item));
}

void KxTreeListItem::Remove()
{
	m_Control->DeleteItem(m_Item);
}
void KxTreeListItem::RemoveChildren()
{
	for (wxTreeListItem i = m_Control->GetFirstChild(m_Item); i.IsOk(); i = m_Control->GetNextSibling(i))
	{
		m_Control->DeleteItem(i);
	}
}
void KxTreeListItem::SetChildrenExpanded(bool b)
{
	for (wxTreeListItem i = m_Control->GetFirstChild(m_Item); i.IsOk(); i = m_Control->GetNextSibling(i))
	{
		b ? m_Control->Expand(i) : m_Control->Collapse(i);
	}
}
bool KxTreeListItem::IsExpanded() const
{
	return m_Control->IsExpanded(m_Item);
}
void KxTreeListItem::SetExpanded(bool b)
{
	b ? m_Control->Expand(m_Item) : m_Control->Collapse(m_Item);
}
void KxTreeListItem::EnsureVisible()
{
	m_Control->EnsureVisible(m_Item);
}
void KxTreeListItem::SetFocus()
{
	if (m_Control->m_DataView)
	{
		if (m_Control->m_DataView->HasFlag(wxDV_MULTIPLE))
		{
			m_Control->m_DataView->SetCurrentItem(m_Control->GetItemDV(m_Item));
		}
	}
}
bool KxTreeListItem::IsSelected() const
{
	return m_Control->IsSelected(m_Item);
}
void KxTreeListItem::SetSelection(bool b)
{
	b ? m_Control->Select(m_Item) : m_Control->Unselect(m_Item);
}
int KxTreeListItem::GetIndexWithinParent() const
{
	wxTreeListItem parent = m_Control->GetItemParent(m_Item);
	if (parent.IsOk())
	{
		wxTreeListItem item = m_Control->GetFirstChild(parent);
		if (item.IsOk())
		{
			int index = 0;
			while (item.IsOk() && item.GetID() != m_Item.GetID())
			{
				index++;
				item = m_Control->GetNextSibling(item);
			}
			return index;
		}
	}
	return -1;
}

KxStringVector KxTreeListItem::GetLabels() const
{
	KxStringVector tLables;
	tLables.resize(m_Control->GetColumnCount());
	for (unsigned int i = 0; i < m_Control->GetColumnCount(); i++)
	{
		tLables[i] = m_Control->GetItemText(m_Item, i);
	}
	return tLables;
}
void KxTreeListItem::SetLabels(const KxStringVector& labels)
{
	size_t i = 0;
	for (i = 0; i < labels.size(); i++)
	{
		SetLabel(labels[i], i);
	}
	for (; i < m_Control->GetColumnCount(); i++)
	{
		SetLabel(wxEmptyString, i);
	}
}
wxString KxTreeListItem::GetLabel(size_t i) const
{
	return m_Control->GetItemText(m_Item, i);
}
void KxTreeListItem::SetLabel(const wxString& s, size_t i)
{
	m_Control->SetItemText(m_Item, i, s);
}
void KxTreeListItem::BeginEditLabel(size_t i)
{
	if (m_Control->m_DataView)
	{
		m_Control->m_DataView->EditItem(m_Control->GetItemDV(m_Item), m_Control->GetColumnDV(i));
	}
}
void KxTreeListItem::EndEditLabel(bool discardChnages, size_t i)
{
	auto renderer = m_Control->GetColumnRendererDV(i);
	if (renderer)
	{
		if (discardChnages)
		{
			renderer->CancelEditing();
		}
		else
		{
			renderer->FinishEditing();
		}
	}
}
int KxTreeListItem::GetImage(KxTreeList_ImageState type) const
{
	return m_Control->GetItemImage(m_Item, type);
}
void KxTreeListItem::SetImage(int collapsed, int expanded)
{
	if (collapsed != DONT_CHANGE)
	{
		m_Control->SetItemImage(m_Item, collapsed, KxTL_IMAGE_COLLAPSED);
	}

	if (expanded == SAME)
	{
		int nImage = collapsed >= 0 ? collapsed : GetImage(KxTL_IMAGE_COLLAPSED);
		m_Control->SetItemImage(m_Item, nImage, KxTL_IMAGE_EXPANDED);
	}
	else if (expanded != DONT_CHANGE)
	{
		m_Control->SetItemImage(m_Item, expanded, KxTL_IMAGE_EXPANDED);
	}
}
wxClientData* KxTreeListItem::GetData() const
{
	return m_Control->GetItemData(m_Item);
}
void KxTreeListItem::SetData(wxClientData* p)
{
	m_Control->SetItemData(m_Item, p);
}
bool KxTreeListItem::IsChecked() const
{
	return m_Control->GetCheckedState(m_Item) == wxCHK_CHECKED;
}
void KxTreeListItem::SetChecked(bool b)
{
	m_Control->CheckItem(m_Item, b ? wxCHK_CHECKED : wxCHK_UNCHECKED);
}
wxCheckBoxState KxTreeListItem::GetChecked() const
{
	return m_Control->GetCheckedState(m_Item);
}
void KxTreeListItem::SetChecked(wxCheckBoxState v)
{
	m_Control->CheckItem(m_Item, v);
}

void KxTreeListItem::Swap(KxTreeListItem& other)
{
	auto oldNode = m_Item.m_pItem;
	other.m_Item.m_pItem = m_Item.m_pItem;
	m_Item.m_pItem = oldNode;

	for (unsigned int i = 0; i < m_Control->GetColumnCount(); i++)
	{
		m_Control->m_DataView->GetModel()->ValueChanged(m_Control->GetItemDV(m_Item), i);
		m_Control->m_DataView->GetModel()->ValueChanged(m_Control->GetItemDV(other.m_Item), i);
	}
}
wxRect KxTreeListItem::GetRect() const
{
	return m_Control->GetItemRect(m_Item);
}
