#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewEditor.h"
class wxColourDialog;

class KxDataViewColorEditor: public KxDataViewEditor
{
	public:
		static bool GetValueAsColor(const wxAny& value, KxColor& color);

	private:
		wxColourDialog* m_Dialog = NULL;
		KxColor m_Color;

	protected:
		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;
};
