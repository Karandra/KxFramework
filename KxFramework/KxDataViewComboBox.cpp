#include "KxStdAfx.h"
#include "KxFramework/KxDataViewComboBox.h"
#include "KxFramework/DataView2/MainWindow.h"
#include "KxFramework/KxComboControl.h"
#include "KxFramework/KxPanel.h"

namespace KxDataView2
{
	KxEVENT_DEFINE_GLOBAL(Event, DVCB_GET_STRING_VALUE);
	KxEVENT_DEFINE_GLOBAL(Event, DVCB_SET_STRING_VALUE);
}

namespace KxDataView2
{
	wxIMPLEMENT_ABSTRACT_CLASS(ComboBoxCtrl, View);

	int ComboBoxCtrl::CalculateItemsHeight() const
	{
		int height = 0;
		if (m_MaxVisibleItems != -1)
		{
			if (const wxHeaderCtrl* header = GetHeaderCtrl())
			{
				height += header->GetSize().GetHeight() + FromDIP(4);
			}

			int items = std::min<int>(m_MaxVisibleItems, GetMainWindow()->GetRowCount());
			height += (items * FromDIP(4)) + (items * GetUniformRowHeight());
		}
		else
		{
			height = GetUniformRowHeight();
		}
		return std::clamp(height, 0, wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	}
	void ComboBoxCtrl::UpdatePopupHeight()
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

	void ComboBoxCtrl::OnInternalIdle()
	{
		View::OnInternalIdle();
	}
	void ComboBoxCtrl::OnSelectItem(Event& event)
	{
		event.Skip();

		m_Selection = event.GetNode();
		if (IsOptionEnabled(KxDVCB_OPTION_DISMISS_ON_SELECT))
		{
			m_ComboCtrl->Dismiss();
		}
	}
	void ComboBoxCtrl::OnScroll(wxMouseEvent& event)
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

	bool ComboBoxCtrl::FindItem(const wxString& value, wxString* pTrueItem)
	{
		return true;
	}
	void ComboBoxCtrl::OnPopup()
	{
		SetFocus();
	}
	void ComboBoxCtrl::OnDismiss()
	{
		if (m_Selection)
		{
			Select(*m_Selection);
		}

		if (IsOptionEnabled(KxDVCB_OPTION_FORCE_GET_STRING_VALUE_ON_DISMISS))
		{
			m_ComboCtrl->SetText(GetStringValue());
		}
	}
	void ComboBoxCtrl::OnDoShowPopup()
	{
		UpdatePopupHeight();
	}
	bool ComboBoxCtrl::Create(wxWindow* window)
	{
		m_Sizer = new wxBoxSizer(IsOptionEnabled(KxDVCB_OPTION_HORIZONTAL_SIZER) ? wxHORIZONTAL : wxVERTICAL);
		m_BackgroundWindow = new KxPanel(window, wxID_NONE, KxPanel::DefaultStyle|wxBORDER_THEME);
		m_BackgroundWindow->SetSizer(m_Sizer);
		if (ShouldInheritColours())
		{
			m_BackgroundWindow->SetBackgroundColour(m_ComboCtrl->GetBackgroundColour());
			m_BackgroundWindow->SetForegroundColour(m_ComboCtrl->GetForegroundColour());
		}

		if (View::Create(m_BackgroundWindow, wxID_NONE, m_DataViewFlags))
		{
			View::SetPosition(wxPoint(0, 0));
			m_Sizer->Add(this, 1, wxEXPAND);

			// DataView events
			m_EvtHandler_DataView.Bind(EvtITEM_SELECTED, &ComboBoxCtrl::OnSelectItem, this);
			PushEventHandler(&m_EvtHandler_DataView);

			// ComboCtrl Events
			m_ComboCtrl->PushEventHandler(&m_EvtHandler_ComboCtrl);
			if (!IsOptionEnabled(KxDVCB_OPTION_ALT_POPUP_WINDOW))
			{
				m_EvtHandler_ComboCtrl.Bind(wxEVT_MOUSEWHEEL, &ComboBoxCtrl::OnScroll, this);
			}
			return true;
		}
		return false;
	}

	bool ComboBoxCtrl::Create(wxWindow* parent,
									wxWindowID id,
									long style,
									const wxValidator& validator
	)
	{
		m_ComboCtrl = new KxComboControl();
		m_ComboCtrl->UseAltPopupWindow(IsOptionEnabled(KxDVCB_OPTION_ALT_POPUP_WINDOW));

		if (m_ComboCtrl->Create(parent, id, {}, style, validator))
		{
			m_ComboCtrl->SetFocusDrawMode(KxComboControl::DrawFocus::Never);
			m_ComboCtrl->SetPopupControl(this);

			return true;
		}
		return false;
	}
	ComboBoxCtrl::~ComboBoxCtrl()
	{
		PopEventHandler();
		m_ComboCtrl->PopEventHandler();
	}

	wxWindow* ComboBoxCtrl::GetControl()
	{
		return m_BackgroundWindow;
	}
	wxComboCtrl* ComboBoxCtrl::GetComboControl()
	{
		return m_ComboCtrl;
	}
	wxWindow* ComboBoxCtrl::ComboGetBackgroundWindow()
	{
		return m_BackgroundWindow;
	}

	wxString ComboBoxCtrl::GetStringValue() const
	{
		Event event(KxEVT_DVCB_GET_STRING_VALUE, GetId());
		event.SetEventObject(const_cast<ComboBoxCtrl*>(this));
		event.SetNode(m_Selection);

		HandleWindowEvent(event);
		return event.GetString();
	}
	void ComboBoxCtrl::SetStringValue(const wxString& value)
	{
		Event event(KxEVT_DVCB_SET_STRING_VALUE, GetId());
		event.SetEventObject(const_cast<ComboBoxCtrl*>(this));
		event.SetString(value);

		HandleWindowEvent(event);
		if (Node* node = event.GetNode())
		{
			Select(*node);
		}
	}

	void ComboBoxCtrl::ComboPopup()
	{
		m_ComboCtrl->Popup();
	}
	void ComboBoxCtrl::ComboDismiss()
	{
		m_ComboCtrl->Dismiss();
	}
	void ComboBoxCtrl::ComboRefreshLabel()
	{
		m_ComboCtrl->SetText(GetStringValue());
	}

	int ComboBoxCtrl::ComboGetMaxVisibleItems() const
	{
		return m_MaxVisibleItems;
	}
	void ComboBoxCtrl::ComboSetMaxVisibleItems(int count)
	{
		m_MaxVisibleItems = count;
	}

	void ComboBoxCtrl::ComboSetPopupExtents(int nLeft, int nRight)
	{
		m_ComboCtrl->SetPopupExtents(nLeft, nRight);
	}
	void ComboBoxCtrl::ComboSetPopupMinWidth(int width)
	{
		m_ComboCtrl->SetPopupMinWidth(width);
	}
	void ComboBoxCtrl::ComboSetPopupAnchor(wxDirection nSide)
	{
		m_ComboCtrl->SetPopupAnchor(nSide);
	}
}
