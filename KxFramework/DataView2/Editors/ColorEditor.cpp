#include "KxStdAfx.h"
#include "ColorEditor.h"
#include <wx/colordlg.h>
#include <wx/generic/colrdlgg.h>

namespace
{
	template<typename> struct ArraySize;
	template<typename T, size_t N> struct ArraySize<std::array<T, N>>
	{
		static constexpr size_t size = N;
	};
}

namespace KxDataView2
{
	ColorValue::ColorValue(const KxColor& color):m_Color(color)
	{
		static_assert(wxColourData::NUM_CUSTOM == ArraySize<decltype(m_PaletteColors)>::size);
	}

	bool ColorValue::FromAny(const wxAny& value)
	{
		if (value.GetAs(this))
		{
			return true;
		}
		else if (KxColor color; value.GetAs(&color))
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

		colorData.SetChooseFull(IsOptionEnabled(Options::FullEditor));
		colorData.SetChooseAlpha(IsOptionEnabled(Options::ShowAlpha));

		return colorData;
	}
	void ColorValue::FromColorData(const wxColourData& colorData)
	{
		m_Color = colorData.GetColour();
		for (size_t i = 0; i < m_PaletteColors.size(); i++)
		{
			m_PaletteColors[i] = colorData.GetCustomColour(i);
		}

		SetOptionEnabled(Options::FullEditor, colorData.GetChooseFull());
		SetOptionEnabled(Options::ShowAlpha, colorData.GetChooseAlpha());
	}
}

namespace KxDataView2
{
	wxWindow* ColorEditor::CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
	{
		m_Value = FromAnyUsing<ColorValue>(value);
		wxColourData colorData = m_Value.ToColorData();

		wxColourDialog* nativeDialog = nullptr;
		wxGenericColourDialog* genericDialog = nullptr;
		const bool shouldUseGeneric = m_Value.IsOptionEnabled(Options::GenericEditor) || m_Value.IsOptionEnabled(Options::ShowAlpha);

		if (shouldUseGeneric)
		{
			genericDialog = new wxGenericColourDialog(parent, &colorData);
			m_Dialog = genericDialog;
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
					m_Value.FromColorData(genericDialog->GetColourData());
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
