#include "KxStdAfx.h"
#include "ColorEditor.h"
#include <wx/colordlg.h>
#include <wx/generic/colrdlgg.h>

namespace KxDataView2
{
	bool ColorEditor::GetValueAsColor(const wxAny& value, KxColor& color)
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

	wxWindow* ColorEditor::CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
	{
		bool shouldUseGeneric = IsOptionEnabled(ColorEditorStyle::UseGeneric) || IsOptionEnabled(ColorEditorStyle::ShowAlpha);

		m_ColorData.SetColour(GetValueAsColor(value));
		m_ColorData.SetChooseFull(IsOptionEnabled(ColorEditorStyle::Full));
		m_ColorData.SetChooseAlpha(IsOptionEnabled(ColorEditorStyle::ShowAlpha));

		wxColourDialog* nativeDialog = nullptr;
		wxGenericColourDialog* genericDialog = nullptr;

		if (shouldUseGeneric)
		{
			genericDialog = new wxGenericColourDialog(parent, &m_ColorData);
			m_Dialog = genericDialog;
		}
		else
		{
			nativeDialog = new wxColourDialog(parent, &m_ColorData);
			m_Dialog = nativeDialog;
		}

		wxTheApp->CallAfter([this, nativeDialog, genericDialog]()
		{
			if (m_Dialog->ShowModal() == wxID_OK)
			{
				if (nativeDialog)
				{
					m_ColorData = nativeDialog->GetColourData();
				}
				else if (genericDialog)
				{
					m_ColorData = genericDialog->GetColourData();
				}
				EndEdit();
			}
		});
		return m_Dialog;
	}
	wxAny ColorEditor::GetValue(wxWindow* control) const
	{
		control->Destroy();

		KxColor color = m_ColorData.GetColour();
		if (color.IsOk())
		{
			return color;
		}
		return {};
	}

	wxColour ColorEditor::GetCustomColor(size_t index) const
	{
		if (index < wxColourData::NUM_CUSTOM)
		{
			m_ColorData.GetCustomColour(index);
		}
		return {};
	}
	void ColorEditor::SetCustomColor(size_t index, const wxColour& color)
	{
		if (index < wxColourData::NUM_CUSTOM)
		{
			m_ColorData.SetCustomColour(index, color);
		}
	}
}
