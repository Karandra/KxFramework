#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewColorEditor.h"
#include <wx/colordlg.h>

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

	m_Dialog = new wxColourDialog(parent, &colorData);
	GetView()->CallAfter([this]()
	{
		if (m_Dialog->ShowModal() == wxID_OK)
		{
			m_Color = m_Dialog->GetColourData().GetColour();
			EndEdit();
		}
	});
	return m_Dialog;
}
bool KxDataViewColorEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	control->Destroy();
	value = m_Color;
	return true;
}
