#include "stdafx.h"
#include "DateEditor.h"
#include "Kx/Utility/Common.h"
#include <wx/datectrl.h>

namespace
{
	int ConvertControlStyle(const KxFramework::UI::DataView::DateTimeValue& value)
	{
		using namespace KxFramework;
		using namespace KxFramework::UI::DataView;

		int style = 0;
		Utility::AddFlagRef(style, wxDP_SPIN, value.IsOptionEnabled(DateEditorOption::Spin));
		Utility::AddFlagRef(style, wxDP_DROPDOWN, value.IsOptionEnabled(DateEditorOption::Dropdown));
		Utility::AddFlagRef(style, wxDP_ALLOWNONE, value.IsOptionEnabled(DateEditorOption::AllowNone));
		Utility::AddFlagRef(style, wxDP_SHOWCENTURY, value.IsOptionEnabled(DateEditorOption::ShowCentury));

		return style;
	}
}

namespace KxFramework::UI::DataView
{
	bool DateTimeValue::FromAny(const wxAny& value)
	{
		if (value.GetAs(&m_Value) || value.GetAs(this))
		{
			return true;
		}
		else if (SYSTEMTIME systemTime; value.CheckType<SYSTEMTIME>() && value.GetAs(&systemTime))
		{
			m_Value.SetSystemTime(systemTime);
			return true;
		}
		else if (FILETIME fileTime; value.CheckType<FILETIME>() && value.GetAs(&fileTime))
		{
			if (::FileTimeToSystemTime(&fileTime, &systemTime))
			{
				m_Value.SetSystemTime(systemTime);
				return true;
			}
		}
		else if (time_t unixTime = 0; value.CheckType<time_t>() && value.GetAs(&unixTime))
		{
			m_Value.SetUnixTime(unixTime);
			return true;
		}
		else if (wxString string; value.GetAs(&string))
		{
			return m_Value.ParseISOCombined(string) || m_Value.ParseISOCombined(string, wxS(' ')) || m_Value.ParseTime(string);
		}
		return false;
	}
}

namespace KxFramework::UI::DataView
{
	wxWindow* DateEditor::CreateControl(wxWindow* parent, const Rect& cellRect, const wxAny& value)
	{
		const DateTimeValue dateTimeValue = FromAnyUsing<DateTimeValue>(value);
		const int style = ConvertControlStyle(dateTimeValue);

		wxDatePickerCtrl* editor = new wxDatePickerCtrl(parent,
														wxID_NONE,
														dateTimeValue.GetDateTime(),
														cellRect.GetPosition(),
														cellRect.GetSize(),
														style,
														GetValidator()
		);

		if (dateTimeValue.HasDateRange())
		{
			auto [lower, upper] = dateTimeValue.GetDateTimeRange();
			editor->SetRange(lower, upper);
		}
		return editor;
	}
	wxAny DateEditor::GetValue(wxWindow* control) const
	{
		wxDatePickerCtrl* editor = static_cast<wxDatePickerCtrl*>(control);
		return editor->GetValue().ResetTime();
	}
}
