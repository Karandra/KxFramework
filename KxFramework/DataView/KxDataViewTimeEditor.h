#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewEditor.h"

class KX_API KxDataViewTimeEditor: public KxDataViewEditor
{
	protected:
		wxDateTime& ResetDate(wxDateTime& date) const
		{
			date.SetDay(0);
			date.SetMonth(wxDateTime::Inv_Month);
			date.SetYear(wxDateTime::Inv_Year);

			return date;
		}

		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;
};
