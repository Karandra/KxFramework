#include "KxStdAfx.h"
#include "DateEditor.h"
#include <wx/datectrl.h>

namespace
{
	int ConvertControlStyle(const Kx::DataView2::DateEditor& editor)
	{
		using DateEditorStyle = Kx::DataView2::DateEditorStyle;

		int style = 0;
		KxUtility::ModFlagRef(style, wxDP_SPIN, editor.IsOptionEnabled(DateEditorStyle::Spin));
		KxUtility::ModFlagRef(style, wxDP_DROPDOWN, editor.IsOptionEnabled(DateEditorStyle::Dropdown));
		KxUtility::ModFlagRef(style, wxDP_ALLOWNONE, editor.IsOptionEnabled(DateEditorStyle::AllowNone));
		KxUtility::ModFlagRef(style, wxDP_SHOWCENTURY, editor.IsOptionEnabled(DateEditorStyle::ShowCentury));

		return style;
	}
}

namespace Kx::DataView2
{
	bool DateEditor::GetValueAsDateTime(const wxAny& value, wxDateTime& dateTime)
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

		FILETIME fileTime = {0};
		if (value.CheckType<FILETIME>() && value.GetAs(&fileTime))
		{
			if (::FileTimeToSystemTime(&fileTime, &systemTime))
			{
				dateTime.SetFromMSWSysTime(systemTime);
				return true;
			}
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
			return dateTime.ParseISOCombined(string) || dateTime.ParseISOCombined(string, wxS(' ')) || dateTime.ParseTime(string);
		}
		return false;
	}

	wxWindow* DateEditor::CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
	{
		const int style = ConvertControlStyle(*this);
		wxDatePickerCtrl* editor = new wxDatePickerCtrl(parent, wxID_NONE, GetValueAsDateTime(value), cellRect.GetPosition(), cellRect.GetSize(), style, GetValidator());

		if (m_Min.IsValid() && m_Max.IsValid())
		{
			editor->SetRange(m_Min, m_Max);
		}
		return editor;
	}
	wxAny DateEditor::GetValue(wxWindow* control) const
	{
		wxDatePickerCtrl* editor = static_cast<wxDatePickerCtrl*>(control);
		return editor->GetValue().ResetTime();
	}
}
