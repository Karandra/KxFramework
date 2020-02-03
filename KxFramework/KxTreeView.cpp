#include "KxStdAfx.h"
#include "KxFramework/KxImageList.h"
#include "KxFramework/KxTreeView.h"
#include "KxFramework/KxTextBox.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxIncludeWindows.h"

KxEVENT_DEFINE_GLOBAL(wxTreeEvent, TREE_ITEM_CHECKED);

wxIMPLEMENT_DYNAMIC_CLASS(KxTreeView, wxTreeCtrl);

wxImageList KxTreeView::ms_CheckBoxIcons;
bool KxTreeView::ms_CheckBoxIconsInitialized = false;
void KxTreeView::InitCheckBoxImages(wxWindow* window)
{
	if (!ms_CheckBoxIconsInitialized)
	{
		ms_CheckBoxIconsInitialized = true;

		wxSize size = wxSize(16, 16);
		wxRect rect(wxPoint(0, 0), size);
		ms_CheckBoxIcons.Create(size.GetWidth(), size.GetHeight(), false, 3);

		wxBitmap bitmap(size.GetWidth(), size.GetHeight());
		wxMemoryDC dc;
		dc.SelectObjectAsSource(bitmap);

		dc.Clear();
		wxRendererNative::Get().DrawCheckBox(window, dc, rect, 0);
		dc.SelectObject(wxNullBitmap);
		ms_CheckBoxIcons.Add(bitmap);

		dc.SelectObjectAsSource(bitmap);
		dc.Clear();
		wxRendererNative::Get().DrawCheckBox(window, dc, rect, wxCONTROL_CHECKED);
		dc.SelectObject(wxNullBitmap);
		ms_CheckBoxIcons.Add(bitmap);

		dc.SelectObjectAsSource(bitmap);
		dc.Clear();
		wxRendererNative::Get().DrawCheckBox(window, dc, rect, wxCONTROL_UNDETERMINED);
		dc.SelectObject(wxNullBitmap);
		ms_CheckBoxIcons.Add(bitmap);
	}
}

void KxTreeView::OnKillFocus(wxFocusEvent& event)
{
	Refresh();
	wxWindow* window = event.GetWindow();
	if (window)
	{
		window->SetFocus();
	}
	event.Skip();
}
void KxTreeView::OnCheck(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();
	int checkState = GetItemChecked(item);

	// Check custom event
	wxTreeEvent checkEvent = event;
	checkEvent.SetEventType(KxEVT_TREE_ITEM_CHECKED);
	checkEvent.SetInt(KxTV_STATE_UNCHECKED);

	if (IsStateImagesChangeEnabled() && checkState != KxTV_STATE_HIDE)
	{
		KxTreeView_ItemState newCheckState = KxTV_STATE_HIDE;
		switch (checkState)
		{
			case KxTV_STATE_CHECKED:
			{
				newCheckState = KxTV_STATE_UNCHECKED;
				checkEvent.SetInt(KxTV_STATE_UNCHECKED);
				break;
			}
			case KxTV_STATE_UNCHECKED:
			{
				if (Is3StateCheckboxesEnabled())
				{
					newCheckState = KxTV_STATE_INDETERMINATE;
				}
				else
				{
					newCheckState = KxTV_STATE_CHECKED;
					checkEvent.SetInt(KxTV_STATE_CHECKED);
				}
				break;
			}
			case KxTV_STATE_INDETERMINATE:
			{
				if (Is3StateCheckboxesEnabled())
				{
					newCheckState = KxTV_STATE_CHECKED;
				}
				else
				{
					newCheckState = KxTV_STATE_UNCHECKED;
					checkEvent.SetInt(KxTV_STATE_UNCHECKED);
				}
				break;
			}
		}

		if (newCheckState != KxTV_STATE_HIDE)
		{
			SetItemChecked(item, newCheckState);
		}
	}

	event.Skip();
	HandleWindowEvent(checkEvent);
}
void KxTreeView::OnKeyDown(wxTreeEvent& event)
{
	if (event.GetKeyCode() == WXK_SPACE)
	{
		event.SetItem(GetFocusedItem());
		OnCheck(event);
	}
	event.Skip();
}

bool KxTreeView::Create(wxWindow* parent,
						wxWindowID id,
						long style
)
{
	InitCheckBoxImages(parent);
	m_Options = (KxTreeView_Options)style;

	if (wxTreeCtrl::Create(parent, id, wxDefaultPosition, wxDefaultSize, KxUtility::ModFlag(style, KxTV_MASK, false)))
	{
		EnableSystemTheme(true);
		SetDoubleBuffered(false); // Don't enable, causes redraw bug

		SetWindowLongPtrW(GetHandle(), GWL_STYLE, GetWindowLongPtrW(GetHandle(), GWL_STYLE)|TVS_NONEVENHEIGHT);
		SetWindowLongPtrW(GetHandle(), GWL_EXSTYLE, GetWindowLongPtrW(GetHandle(), GWL_EXSTYLE)|TVS_EX_DRAWIMAGEASYNC);
		SetItemHeight(DefaultItemHeight);

		if (m_Options & KxTV_HOTTARCK_ENABLED)
		{
			SetHotTrackEnabled(true);
		}
		if (m_Options & KxTV_BUTTONS_FADE_ENABLED)
		{
			SetFadeButtonsEnabled(true);
		}
		if (m_Options & KxTV_CHK_ENABLED)
		{
			SetCheckBoxesEnabled(true);
		}

		Bind(wxEVT_KILL_FOCUS, &KxTreeView::OnKillFocus, this);
		Bind(wxEVT_TREE_STATE_IMAGE_CLICK, &KxTreeView::OnCheck, this);
		Bind(wxEVT_TREE_KEY_DOWN, &KxTreeView::OnKeyDown, this);
		return true;
	}
	return false;
}
KxTreeView::~KxTreeView()
{
}

int KxTreeView::GetItemHeight() const
{
	return SendMessageW(GetHandle(), TVM_GETITEMHEIGHT, 0, 0);
}
void KxTreeView::SetItemHeight(int height)
{
	SendMessageW(GetHandle(), TVM_SETITEMHEIGHT, height, 0);
}
void KxTreeView::SetHotTrackEnabled(bool enable)
{
	KxUtility::ToggleWindowStyle(GetHandle(), GWL_STYLE, TVS_TRACKSELECT, enable);
	KxUtility::ModFlagRef(m_Options, KxTV_HOTTARCK_ENABLED, enable);
}
void KxTreeView::SetFadeButtonsEnabled(bool enable)
{
	if (enable)
	{
		SendMessageW(GetHandle(), TVM_SETEXTENDEDSTYLE, TVS_EX_FADEINOUTEXPANDOS, TVS_EX_FADEINOUTEXPANDOS);
	}
	else
	{
		SendMessageW(GetHandle(), TVM_SETEXTENDEDSTYLE, 0, TVS_EX_FADEINOUTEXPANDOS);
	}
	KxUtility::ModFlagRef(m_Options, KxTV_BUTTONS_FADE_ENABLED, enable);
}

KxTreeViewItem KxTreeView::InsertRoot(const wxString& label, wxTreeItemData* data)
{
	return KxTreeViewItem(*this, wxTreeCtrl::AddRoot(label, KxTreeViewItem::NO_IMAGE, KxTreeViewItem::NO_IMAGE, data));
}
KxTreeViewItem KxTreeView::GetRoot() const
{
	return KxTreeViewItem(*const_cast<KxTreeView*>(this), wxTreeCtrl::GetRootItem());
}
KxTreeViewItem KxTreeView::GetFirstVisible() const
{
	return KxTreeViewItem(*const_cast<KxTreeView*>(this), wxTreeCtrl::GetFirstVisibleItem());
}
KxTreeViewItem KxTreeView::GetFocused() const
{
	return KxTreeViewItem(*const_cast<KxTreeView*>(this), wxTreeCtrl::GetFocusedItem());
}
KxTreeViewItem KxTreeView::GetSelectedItem() const
{
	return KxTreeViewItem(*const_cast<KxTreeView*>(this), wxTreeCtrl::GetSelection());
}
KxTreeViewItem KxTreeView::HitTestItem(const wxPoint& pos, int& flags) const
{
	return KxTreeViewItem(*const_cast<KxTreeView*>(this), wxTreeCtrl::HitTest(pos, flags));
}

//////////////////////////////////////////////////////////////////////////
void KxTreeViewItem::OnCreate()
{
	m_Control->SetItemState(m_Item, KxTV_STATE_UNCHECKED);
}

KxTreeViewItem::KxTreeViewItem(KxTreeView& control, const wxTreeItemId& v)
	:m_Control(&control), m_Item(v)
{
	OnCreate();
}
void KxTreeViewItem::Create(KxTreeView& control, const wxTreeItemId& v)
{
	m_Control = &control;
	m_Item = v;
}

KxTreeViewItem KxTreeViewItem::Add(const wxString& label, wxTreeItemData* data)
{
	return KxTreeViewItem(*m_Control, m_Control->AppendItem(m_Item, label, NO_IMAGE, NO_IMAGE, data));
}
KxTreeViewItem KxTreeViewItem::Insert(const wxString& label, KxTreeViewItem& previous, wxTreeItemData* data)
{
	return KxTreeViewItem(*m_Control, m_Control->InsertItem(m_Item, previous, label, NO_IMAGE, NO_IMAGE, data));
}
KxTreeViewItem KxTreeViewItem::Insert(const wxString& label, size_t index, wxTreeItemData* data)
{
	return KxTreeViewItem(*m_Control, m_Control->InsertItem(m_Item, index, label, NO_IMAGE, NO_IMAGE, data));
}

void KxTreeViewItem::Remove()
{
	m_Control->Delete(m_Item);
}
void KxTreeViewItem::RemoveChildren()
{
	m_Control->DeleteChildren(m_Item);
}
void KxTreeViewItem::SortChildren()
{
	m_Control->SortChildren(m_Item);
}
void KxTreeViewItem::ExpandChildren()
{
	m_Control->ExpandAllChildren(m_Item);
}
void KxTreeViewItem::CollapseChildren()
{
	m_Control->CollapseAllChildren(m_Item);
}
void KxTreeViewItem::SetExpanded(bool expanded)
{
	if (expanded)
	{
		m_Control->Expand(m_Item);
	}
	else
	{
		m_Control->Collapse(m_Item);
	}
}
void KxTreeViewItem::EnsureVisible()
{
	m_Control->EnsureVisible(m_Item);
}
void KxTreeViewItem::SetFocus()
{
	m_Control->SetFocusedItem(m_Item);
}
void KxTreeViewItem::SetSelection(bool b)
{
	m_Control->SelectItem(m_Item, b);
}
void KxTreeViewItem::SetHasChildren(bool b)
{
	m_Control->SetItemHasChildren(m_Item, b);
}

wxFont KxTreeViewItem::GetFont() const
{
	return m_Control->GetItemFont(m_Item);
}
void KxTreeViewItem::SetFont(const wxFont& f)
{
	m_Control->SetItemFont(m_Item, f);
}
wxColour KxTreeViewItem::GetBackColor() const
{
	return m_Control->GetItemBackgroundColour(m_Item);
}
void KxTreeViewItem::SetBackColor(const wxColour& c)
{
	m_Control->SetItemBackgroundColour(m_Item, c);
}
wxColour KxTreeViewItem::GetForeColor() const
{
	return m_Control->GetItemTextColour(m_Item);
}
void KxTreeViewItem::SetForeColor(const wxColour& c)
{
	m_Control->SetItemTextColour(m_Item, c);
}

KxTreeViewItem KxTreeViewItem::GetFirstChild(wxTreeItemIdValue& cookie) const
{
	return KxTreeViewItem(*m_Control, m_Control->GetFirstChild(m_Item, cookie));
}
KxTreeViewItem KxTreeViewItem::GetNextChild(wxTreeItemIdValue& cookie) const
{
	return KxTreeViewItem(*m_Control, m_Control->GetNextChild(m_Item, cookie));
}
KxTreeViewItem KxTreeViewItem::GetNextVisible() const
{
	return KxTreeViewItem(*m_Control, m_Control->GetNextVisible(m_Item));
}
KxTreeViewItem KxTreeViewItem::GetPrevSibling() const
{
	return KxTreeViewItem(*m_Control, m_Control->GetPrevSibling(m_Item));
}
KxTreeViewItem KxTreeViewItem::GetNextSibling() const
{
	return KxTreeViewItem(*m_Control, m_Control->GetNextSibling(m_Item));
}
KxTreeViewItem KxTreeViewItem::GetParent() const
{
	return KxTreeViewItem(*m_Control, m_Control->GetItemParent(m_Item));
}

wxString KxTreeViewItem::GetLabel() const
{
	return m_Control->GetItemText(m_Item);
}
void KxTreeViewItem::SetLabel(const wxString& s)
{
	m_Control->SetItemText(m_Item, s);
}
KxTextBox* KxTreeViewItem::BeginEditLabel()
{
	return static_cast<KxTextBox*>(m_Control->EditLabel(m_Item, &KxTextBox::ms_classInfo));
}
void KxTreeViewItem::EndEditLabel(bool discardChnages)
{
	m_Control->EndEditLabel(m_Item, discardChnages);
}
int KxTreeViewItem::GetState() const
{
	return m_Control->GetItemState(m_Item);
}
void KxTreeViewItem::SetState(int state)
{
	m_Control->SetItemState(m_Item, state);
}
int KxTreeViewItem::GetImage(wxTreeItemIcon type) const
{
	return m_Control->GetItemImage(m_Item, type);
}
void KxTreeViewItem::SetImage(wxTreeItemIcon type, int nImage)
{
	m_Control->SetItemImage(m_Item, nImage, type);
}
bool KxTreeViewItem::IsBold() const
{
	return m_Control->IsBold(m_Item);
}
void KxTreeViewItem::SetBold(bool b)
{
	m_Control->SetItemBold(m_Item, b);
}
wxTreeItemData* KxTreeViewItem::GetData() const
{
	return m_Control->GetItemData(m_Item);
}
void KxTreeViewItem::SetData(wxTreeItemData* p)
{
	m_Control->SetItemData(m_Item, p);
}
bool KxTreeViewItem::IsChecked() const
{
	return m_Control->IsCheckBoxesEnabled() && m_Control->GetItemChecked(m_Item) == KxTV_STATE_CHECKED;
}
void KxTreeViewItem::SetChecked(bool b)
{
	if (m_Control->IsCheckBoxesEnabled())
	{
		m_Control->SetItemChecked(m_Item, b);
	}
}
wxCheckBoxState KxTreeViewItem::GetChecked() const
{
	if (m_Control->IsCheckBoxesEnabled())
	{
		switch (m_Control->GetItemChecked(m_Item))
		{
			case KxTV_STATE_CHECKED:
			{
				return wxCHK_CHECKED;
			}
			case KxTV_STATE_INDETERMINATE:
			{
				return wxCHK_UNDETERMINED;
			}
		};
	}
	return wxCHK_UNCHECKED;
}
void KxTreeViewItem::SetChecked(wxCheckBoxState v)
{
	if (m_Control->IsCheckBoxesEnabled())
	{
		KxTreeView_ItemState state = KxTV_STATE_UNCHECKED;
		switch (v)
		{
			case wxCHK_CHECKED:
			{
				state = KxTV_STATE_CHECKED;
				break;
			}
			case wxCHK_UNDETERMINED:
			{
				state = KxTV_STATE_INDETERMINATE;
				break;
			}
		};
		m_Control->SetItemChecked(m_Item, state);
	}
}
