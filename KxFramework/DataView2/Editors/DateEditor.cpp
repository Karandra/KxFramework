#include "KxStdAfx.h"
#include "DateEditor.h"
#include "Kx/Utility/Common.h"
#include <wx/datectrl.h>

namespace
{
	int ConvertControlStyle(const KxDataView2::DateTimeValue& value)
	{
		using namespace KxFramework;
		using Option = KxDataView2::DateEditorOptions;

		int style = 0;
		Utility::ModFlagRef(style, wxDP_SPIN, value.IsOptionEnabled(Option::Spin));
		Utility::ModFlagRef(style, wxDP_DROPDOWN, value.IsOptionEnabled(Option::Dropdown));
		Utility::ModFlagRef(style, wxDP_ALLOWNONE, value.IsOptionEnabled(Option::AllowNone));
		Utility::ModFlagRef(style, wxDP_SHOWCENTURY, value.IsOptionEnabled(Option::ShowCentury));

		return style;
	}
}

namespace KxDataView2
{
	bool DateTimeValue::FromAny(const wxAny& value)
	{
		if (value.GetAs(&m_Value) || value.GetAs(this))
		{
			return true;
		}
		else if (SYSTEMTIME systemTime; value.CheckType<SYSTEMTIME>() && value.GetAs(&systemTime))
		{
			m_Value.SetFromMSWSysTime(systemTime);
			return true;
		}
		else if (FILETIME fileTime; value.CheckType<FILETIME>() && value.GetAs(&fileTime))
		{
			if (::FileTimeToSystemTime(&fileTime, &systemTime))
			{
				m_Value.SetFromMSWSysTime(systemTime);
				return true;
			}
		}
		else if (time_t unixTime = 0; value.CheckType<time_t>() && value.GetAs(&unixTime))
		{
			m_Value.Set(unixTime);
			return true;
		}
		else if (wxString string; value.GetAs(&string))
		{
			return m_Value.ParseISOCombined(string) || m_Value.ParseISOCombined(string, wxS(' ')) || m_Value.ParseTime(string);
		}
		return false;
	}
}

namespace KxDataView2
{
	wxWindow* DateEditor::CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
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
