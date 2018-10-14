#include "KxStdAfx.h"
#include "KxFramework/KxSearchBox.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxIncludeWindows.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxSearchBox, wxSearchCtrl)

void KxSearchBox::OnText(wxCommandEvent& event)
{
	if (HasFocus())
	{
		m_Value = GetValue();
	}
	else
	{
		ChangeValue(m_Value);
	}
}
void KxSearchBox::OnEnter(wxCommandEvent& event)
{
	m_Value = GetValue();

	wxCommandEvent searchEvent(event);
	searchEvent.SetEventType(wxEVT_SEARCHCTRL_SEARCH_BTN);
	searchEvent.SetString(m_Value);
	ProcessWindowEvent(searchEvent);
}
void KxSearchBox::OnKillFocus(wxFocusEvent& event)
{
	ChangeValue(m_Value);
	event.Skip();
}
void KxSearchBox::OnCancelSearch(wxCommandEvent& event)
{
	m_Value.clear();
	ChangeValue(m_Value);
	
	event.Skip();
}

bool KxSearchBox::Create(wxWindow* parent,
						 wxWindowID id,
						 const wxString& value,
						 long style,
						 const wxValidator& validator
)
{
	m_Value = value;

	if (wxSearchCtrl::Create(parent, id, value, wxDefaultPosition, wxDefaultSize, style, validator))
	{
		ShowCancelButton(true);
		ShowSearchButton(true);
		SetDescriptiveText(KxTranslation::GetCurrent().GetString(KxID_SEARCH));
		Refresh();

		m_EvtHandler.Bind(wxEVT_TEXT, &KxSearchBox::OnText, this);
		m_EvtHandler.Bind(wxEVT_TEXT_ENTER, &KxSearchBox::OnEnter, this);
		m_EvtHandler.Bind(wxEVT_KILL_FOCUS, &KxSearchBox::OnKillFocus, this);
		m_EvtHandler.Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, &KxSearchBox::OnCancelSearch, this);
		PushEventHandler(&m_EvtHandler);
		return false;
	}
	return true;
}
KxSearchBox::~KxSearchBox()
{
	PopEventHandler();
}
