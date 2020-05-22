#include "stdafx.h"
#include "ColorEditor.h"
#include "Kx/Utility/Common.h"
#include <wx/colordlg.h>

namespace KxFramework::UI::DataView
{
	ColorValue::ColorValue(const Color& color):m_Color(color)
	{
		using namespace KxFramework::Utility;

		static_assert(wxColourData::NUM_CUSTOM == ArraySize<decltype(m_PaletteColors)>::value);
	}

	bool ColorValue::FromAny(const wxAny& value)
	{
		if (value.GetAs(this))
		{
			return true;
		}
		else if (Color color; value.GetAs(&color))
		{
			return true;
		}
		else if (wxColour color; value.GetAs(&color))
		{
			return true;
		}
		else if (wxColourData colorData; value.GetAs(&colorData))
		{
			FromColorData(colorData);
			return true;
		}
		return false;
	}

	wxColourData ColorValue::ToColorData() const
	{
		wxColourData colorData;
		colorData.SetColour(m_Color);
		for (size_t i = 0; i < m_PaletteColors.size(); i++)
		{
			colorData.SetCustomColour(i, m_PaletteColors[i]);
		}

		colorData.SetChooseFull(IsOptionEnabled(ColorEditorOption::FullEditor));
		colorData.SetChooseAlpha(IsOptionEnabled(ColorEditorOption::ShowAlpha));

		return colorData;
	}
	void ColorValue::FromColorData(const wxColourData& colorData)
	{
		m_Color = colorData.GetColour();
		for (size_t i = 0; i < m_PaletteColors.size(); i++)
		{
			m_PaletteColors[i] = colorData.GetCustomColour(i);
		}

		SetOptionEnabled(ColorEditorOption::FullEditor, colorData.GetChooseFull());
		SetOptionEnabled(ColorEditorOption::ShowAlpha, colorData.GetChooseAlpha());
	}
}

namespace KxFramework::UI::DataView
{
	wxWindow* ColorEditor::CreateControl(wxWindow* parent, const Rect& cellRect, const wxAny& value)
	{
		m_Value = FromAnyUsing<ColorValue>(value);
		wxColourData colorData = m_Value.ToColorData();

		wxColourDialog* nativeDialog = nullptr;
		wxGenericColourDialog* genericDialog = nullptr;
		const bool shouldUseGeneric = m_Value.IsOptionEnabled(ColorEditorOption::GenericEditor) || m_Value.IsOptionEnabled(ColorEditorOption::ShowAlpha);

		if (shouldUseGeneric)
		{
			//genericDialog = new wxGenericColourDialog(parent, &colorData);
			//m_Dialog = genericDialog;

			nativeDialog = new wxColourDialog(parent, &colorData);
			m_Dialog = nativeDialog;
		}
		else
		{
			nativeDialog = new wxColourDialog(parent, &colorData);
			m_Dialog = nativeDialog;
		}

		wxTheApp->CallAfter([this, nativeDialog, genericDialog]()
		{
			m_Value.Clear();
			if (m_Dialog->ShowModal() == wxID_OK)
			{
				if (nativeDialog)
				{
					m_Value.FromColorData(nativeDialog->GetColourData());
				}
				else if (genericDialog)
				{
					//m_Value.FromColorData(genericDialog->GetColourData());
					m_Value.FromColorData(nativeDialog->GetColourData());
				}
				EndEdit();
			}
		});
		return m_Dialog;
	}
	wxAny ColorEditor::GetValue(wxWindow* control) const
	{
		control->Destroy();

		if (m_Value.HasColor())
		{
			return m_Value;
		}
		return {};
	}
}
