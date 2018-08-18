#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewDateEditor.h"
#include <wx/datectrl.h>

bool KxDataViewDateEditor::GetValueAsDateTime(const wxAny& value, wxDateTime& dateTime)
{
	if (value.GetAs(&dateTime))
	{
		return true;
	}

	SYSTEMTIME systemTime = {0};
	if (value.CheckType<SYSTEMTIME>() && value.GetAs(&systemTime))
	{
		dateTime.SetFromMSWSysTime(systemTime);
		return true;
	}

	time_t unixTime = 0;
	if (value.CheckType<time_t>() && value.GetAs(&unixTime))
	{
		dateTime.Set(unixTime);
		return true;
	}
	
	wxString string;
	if (value.GetAs(&string))
	{
		return dateTime.ParseISOCombined(string) || dateTime.ParseISOCombined(string, ' ') || dateTime.ParseTime(string);
	}
	return false;
}

wxWindow* KxDataViewDateEditor::CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
{
	wxDateTime date = wxDefaultDateTime;
	GetValueAsDateTime(value, date);
	date.ResetTime();

	wxDatePickerCtrl* editor = new wxDatePickerCtrl(parent, wxID_NONE, date, cellRect.GetPosition(), cellRect.GetSize(), GetOptionFlags(), GetValidator());

	if (m_Min.IsValid() && m_Max.IsValid())
	{
		editor->SetRange(m_Min, m_Max);
	}
	return editor;
}
bool KxDataViewDateEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	wxDatePickerCtrl* editor = static_cast<wxDatePickerCtrl*>(control);
	value = editor->GetValue().ResetTime();
	return true;
}
