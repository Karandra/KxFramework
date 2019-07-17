#include "KxStdAfx.h"
#include "KxFramework/KxDataViewComboBox.h"
#include "KxFramework/KxComboControl.h"
#include "KxFramework/KxPanel.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"

KxEVENT_DEFINE_GLOBAL(DVCB_GET_STRING_VALUE, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DVCB_SET_STRING_VALUE, KxDataViewEvent);

wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewComboBox, KxDataViewCtrl);

int KxDataViewComboBox::CalculateItemsHeight() const
{
	int height = 0;
	if (m_MaxVisibleItems != -1)
	{
		if (const wxHeaderCtrl* header = GetHeaderCtrl())
		{
			height += header->GetSize().GetHeight() + FromDIP(4);
		}

		int items = std::min(m_MaxVisibleItems, (int)GetMainWindow()->GetRowCount());
		height += (items * FromDIP(4)) + (items * GetUniformRowHeight());
	}
	else
	{
		height = GetUniformRowHeight();
	}
	return std::clamp(height, 0, wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
}
void KxDataViewComboBox::UpdatePopupHeight()
{
	if (m_MaxVisibleItems == -1)
	{
		m_ComboCtrl->SetPopupMaxHeight(-1);
	}
	else
	{
		int height = CalculateItemsHeight();

		// Setting max height to zero doesn't seems to work
		m_ComboCtrl->SetPopupMaxHeight(height == 0 ? 1 : height);
	}
}

void KxDataViewComboBox::OnInternalIdle()
{
	KxDataViewCtrl::OnInternalIdle();
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
	UpdatePopupHeight();
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

int KxDataViewComboBox::ComboGetMaxVisibleItems() const
{
	return m_MaxVisibleItems;
}
void KxDataViewComboBox::ComboSetMaxVisibleItems(int count)
{
	m_MaxVisibleItems = count;
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
