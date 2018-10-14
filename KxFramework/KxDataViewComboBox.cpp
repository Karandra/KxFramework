#include "KxStdAfx.h"
#include "KxFramework/KxDataViewComboBox.h"
#include "KxFramework/KxComboControl.h"
#include "KxFramework/KxPanel.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"

wxDEFINE_EVENT(KxEVT_DVCB_GET_STRING_VALUE, KxDataViewEvent);
wxDEFINE_EVENT(KxEVT_DVCB_SET_STRING_VALUE, KxDataViewEvent);

wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewComboBox, KxDataViewCtrl);

size_t KxDataViewComboBox::GetItemsHeight() const
{
	size_t height = GetUniformRowHeight();
	if (m_MaxVisibleItems != -1)
	{
		#if 0
		const KxDataViewMainWindow* mainWindow = GetMainWindow();

		size_t count = mainWindow->GetRowCount();
		for (size_t i = 0; i < count; i++)
		{
			height += mainWindow->GetLineHeight(i);
		}

		// Two additional pixels per row
		height += FromDIP(2) * count;

		if (count == 1)
		{
			height += GetUniformRowHeight() + FromDIP(4);
		}

		if (const wxHeaderCtrl* header = GetHeaderCtrl())
		{
			height += header->GetSize().GetHeight();
		}
		#endif
		
		#if 1
		const KxDataViewModel* pModel = GetModel();
		if (pModel)
		{
			KxDataViewItem::Vector items;
			pModel->GetChildren(KxDataViewItem(), items);
			size_t max = std::min((size_t)m_MaxVisibleItems, items.size());
			for (size_t i = 0; i < max; i++)
			{
				height += GetItemRect(items[i]).GetHeight();
			}

			if (max == 1)
			{
				height += 4;
			}
			else
			{
				// Two additional pixels by row
				height += 2 * max;
			}
			if (!HasHeaderCtrl())
			{
				const wxHeaderCtrl* header = GetHeaderCtrl();
				height += header ? header->GetSize().GetHeight() : 0;
			}
		}
		#endif
	}

	wxSize minSize = m_Sizer->GetMinSize();
	if (height < (size_t)minSize.GetHeight())
	{
		height = minSize.GetHeight();
	}
	return height;
}
void KxDataViewComboBox::UpdatePopupHeight()
{
	if (m_MaxVisibleItems == -1)
	{
		m_ComboCtrl->SetPopupMaxHeight(-1);
	}
	else
	{
		size_t height = GetItemsHeight();

		// Setting max height to zero doesn't seems to work
		m_ComboCtrl->SetPopupMaxHeight(height == 0 ? 1 : height);
	}
}

void KxDataViewComboBox::OnInternalIdle()
{
	KxDataViewCtrl::OnInternalIdle();
	UpdatePopupHeight();
}
void KxDataViewComboBox::OnSelectItem(KxDataViewEvent& event)
{
	event.Skip();

	KxDataViewItem item = event.GetItem();
	if (item.IsOK())
	{
		m_Selection = item;
	}
	if (IsOptionEnabled(KxDVCB_OPTION_DISMISS_ON_SELECT))
	{
		m_ComboCtrl->Dismiss();
	}
}
void KxDataViewComboBox::OnScroll(wxMouseEvent& event)
{
	event.Skip();

	int rateX = 0;
	int rateY = 0;
	GetScrollPixelsPerUnit(&rateX, &rateY);
	wxPoint startPos = GetViewStart();

	wxCoord value = -event.GetWheelRotation();
	if (event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
	{
		Scroll(wxDefaultCoord, startPos.y + (float)value / (rateY != 0 ? rateY : 1));
	}
	else
	{
		Scroll(startPos.x + (float)value / (rateX != 0 ? rateX : 1), wxDefaultCoord);
	}
}

bool KxDataViewComboBox::FindItem(const wxString& value, wxString* pTrueItem)
{
	return true;
}
void KxDataViewComboBox::OnPopup()
{
	UpdatePopupHeight();
	SetFocus();
}
void KxDataViewComboBox::OnDismiss()
{
	if (m_Selection.IsOK())
	{
		Select(m_Selection);
	}

	if (IsOptionEnabled(KxDVCB_OPTION_FORCE_GET_STRING_VALUE_ON_DISMISS))
	{
		m_ComboCtrl->SetText(GetStringValue());
	}
}
void KxDataViewComboBox::OnDoShowPopup()
{
	
}
bool KxDataViewComboBox::Create(wxWindow* window)
{
	m_Sizer = new wxBoxSizer(IsOptionEnabled(KxDVCB_OPTION_HORIZONTAL_SIZER) ? wxHORIZONTAL : wxVERTICAL);
	m_BackgroundWindow = new KxPanel(window, wxID_NONE, KxPanel::DefaultStyle|wxBORDER_THEME);
	m_BackgroundWindow->SetSizer(m_Sizer);
	if (ShouldInheritColours())
	{
		m_BackgroundWindow->SetBackgroundColour(m_ComboCtrl->GetBackgroundColour());
		m_BackgroundWindow->SetForegroundColour(m_ComboCtrl->GetForegroundColour());
	}

	if (KxDataViewCtrl::Create(m_BackgroundWindow, 1, m_DataViewFlags))
	{
		KxDataViewCtrl::SetPosition(wxPoint(0, 0));
		m_Sizer->Add(this, 1, wxEXPAND);

		// DataView events
		m_EvtHandler_DataView.Bind(KxEVT_DATAVIEW_ITEM_SELECTED, &KxDataViewComboBox::OnSelectItem, this);
		PushEventHandler(&m_EvtHandler_DataView);

		// ComboCtrl Events
		m_ComboCtrl->PushEventHandler(&m_EvtHandler_ComboCtrl);
		if (!IsOptionEnabled(KxDVCB_OPTION_ALT_POPUP_WINDOW))
		{
			m_EvtHandler_ComboCtrl.Bind(wxEVT_MOUSEWHEEL, &KxDataViewComboBox::OnScroll, this);
		}
		return true;
	}
	return false;
}

bool KxDataViewComboBox::Create(wxWindow* parent,
								 wxWindowID id,
								 long style,
								 const wxValidator& validator
)
{
	m_ComboCtrl = new KxComboControl();
	m_ComboCtrl->UseAltPopupWindow(IsOptionEnabled(KxDVCB_OPTION_ALT_POPUP_WINDOW));

	if (m_ComboCtrl->Create(parent, id, wxEmptyString, style, validator))
	{
		m_ComboCtrl->SetFocusDrawMode(KxComboControl::DrawFocus::Never);
		m_ComboCtrl->SetPopupControl(this);

		return true;
	}
	return false;
}
KxDataViewComboBox::~KxDataViewComboBox()
{
	PopEventHandler();
	m_ComboCtrl->PopEventHandler();
}

wxWindow* KxDataViewComboBox::GetControl()
{
	return m_BackgroundWindow;
}
wxComboCtrl* KxDataViewComboBox::GetComboControl()
{
	return m_ComboCtrl;
}
wxWindow* KxDataViewComboBox::ComboGetBackgroundWindow()
{
	return m_BackgroundWindow;
}

wxString KxDataViewComboBox::GetStringValue() const
{
	KxDataViewEvent event(KxEVT_DVCB_GET_STRING_VALUE, GetId());
	event.SetEventObject(const_cast<KxDataViewComboBox*>(this));
	event.SetItem(m_Selection);

	HandleWindowEvent(event);
	return event.GetString();
}
void KxDataViewComboBox::SetStringValue(const wxString& value)
{
	KxDataViewEvent event(KxEVT_DVCB_SET_STRING_VALUE, GetId());
	event.SetEventObject(const_cast<KxDataViewComboBox*>(this));
	event.SetString(value);

	HandleWindowEvent(event);
	KxDataViewItem item = event.GetItem();
	if (item.IsOK())
	{
		Select(item);
	}
}

void KxDataViewComboBox::ComboPopup()
{
	m_ComboCtrl->Popup();
}
void KxDataViewComboBox::ComboDismiss()
{
	m_ComboCtrl->Dismiss();
}
void KxDataViewComboBox::ComboRefreshLabel()
{
	m_ComboCtrl->SetText(GetStringValue());
}
void KxDataViewComboBox::ComboSetPopupExtents(int nLeft, int nRight)
{
	m_ComboCtrl->SetPopupExtents(nLeft, nRight);
}
void KxDataViewComboBox::ComboSetPopupMinWidth(int width)
{
	m_ComboCtrl->SetPopupMinWidth(width);
}
void KxDataViewComboBox::ComboSetPopupAnchor(wxDirection nSide)
{
	m_ComboCtrl->SetPopupAnchor(nSide);
}
