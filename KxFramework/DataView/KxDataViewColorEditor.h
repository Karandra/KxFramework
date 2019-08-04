#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewEditor.h"

class KX_API KxDataViewColorEditor: public KxDataViewEditor
{
	public:
		static bool GetValueAsColor(const wxAny& value, KxColor& color);

	private:
		KxColor m_Color;

	protected:
		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;
};
