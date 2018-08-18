#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewTimeEditor.h"
#include "KxFramework/DataView/KxDataViewDateEditor.h"
#include <wx/timectrl.h>

wxWindow* KxDataViewTimeEditor::CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
{
	wxDateTime date = wxDefaultDateTime;
	KxDataViewDateEditor::GetValueAsDateTime(value, date);
	ResetDate(date);

	wxTimePickerCtrl* editor = new wxTimePickerCtrl(parent, wxID_NONE, date, cellRect.GetPosition(), cellRect.GetSize(), 0, GetValidator());
	return editor;
}
bool KxDataViewTimeEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	wxTimePickerCtrl* editor = static_cast<wxTimePickerCtrl*>(control);

	wxDateTime date = editor->GetValue();
	value = ResetDate(date);
	return true;
}
