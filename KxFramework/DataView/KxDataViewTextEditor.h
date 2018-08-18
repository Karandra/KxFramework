#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewEditor.h"

class KxDataViewTextEditor: public KxDataViewEditor
{
	protected:
		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;
};
