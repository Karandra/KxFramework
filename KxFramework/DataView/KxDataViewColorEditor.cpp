#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewColorEditor.h"
#include <wx/generic/colrdlgg.h>

bool KxDataViewColorEditor::GetValueAsColor(const wxAny& value, KxColor& color)
{
	if (value.GetAs(&color))
	{
		return true;
	}

	wxColour colorWx;
	if (value.GetAs(&colorWx))
	{
		color = colorWx;
		return true;
	}
	return false;
}

wxWindow* KxDataViewColorEditor::CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
{
	GetValueAsColor(value, m_Color);

	wxColourData colorData;
	colorData.SetColour(m_Color);
	colorData.SetChooseFull(true);
	colorData.SetChooseAlpha(true);

	wxGenericColourDialog* dialog = new wxGenericColourDialog(parent, &colorData);
	GetView()->CallAfter([this, dialog]()
	{
		if (dialog->ShowModal() == wxID_OK)
		{
			m_Color = dialog->GetColourData().GetColour();
			EndEdit();
		}
		else
		{
			CancelEdit();
		}
	});
	return dialog;
}
bool KxDataViewColorEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	control->Destroy();
	value = m_Color;
	return true;
}
