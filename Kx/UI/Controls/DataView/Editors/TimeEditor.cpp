#include "stdafx.h"
#include "TimeEditor.h"
#include "DateEditor.h"
#include <wx/timectrl.h>

namespace
{
	KxFramework::DateTime ResetDate(KxFramework::DateTime date)
	{
		using namespace KxFramework;

		date.SetDay(0);
		date.SetMonth(Month::None);
		date.SetYear(DateTime::InvalidYear);

		return date;
	}
}

namespace KxFramework::UI::DataView
{
	wxWindow* TimeEditor::CreateControl(wxWindow* parent, const Rect& cellRect, const wxAny& value)
	{
		const DateTimeValue dateValue = FromAnyUsing<DateTimeValue>(value);

		return new wxTimePickerCtrl(parent,
									wxID_NONE,
									ResetDate(dateValue.GetDateTime()),
									cellRect.GetPosition(),
									cellRect.GetSize(),
									0,
									GetValidator()
		);
	}
	wxAny TimeEditor::GetValue(wxWindow* control) const
	{
		wxTimePickerCtrl* editor = static_cast<wxTimePickerCtrl*>(control);
		return ResetDate(editor->GetValue());
	}
}
