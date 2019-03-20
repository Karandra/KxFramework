#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewSpinEditor.h"
#include <wx/spinctrl.h>

namespace
{
	template<class T> T CastAndGetValue(wxWindow* control)
	{
		if constexpr(std::is_floating_point<T>::value)
		{
			return static_cast<wxSpinCtrlDouble*>(control)->GetValue();
		}
		else
		{
			return static_cast<wxSpinCtrl*>(control)->GetValue();
		}
	}
}

wxWindow* KxDataViewSpinEditor::CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
{
	int style = wxSP_ARROW_KEYS|wxTE_PROCESS_ENTER;
	if (m_IsWrapping)
	{
		style |= wxSP_WRAP;
	}
	style |= GetRenderer()->GetEffectiveAlignment();

	wxPoint pos = cellRect.GetTopLeft();
	wxSize size = cellRect.GetSize();

	wxControl* editor = nullptr;
	if (m_Type == Type::Integer)
	{
		int initialValue = 0;
		value.GetAs(&initialValue);

		if (ShouldMimicIntegerUsingFloat())
		{
			wxSpinCtrlDouble* spin = new wxSpinCtrlDouble(parent, wxID_NONE, wxEmptyString, pos, size, style, m_IntMin, m_IntMax, initialValue, m_IntIncrement);
			spin->SetDigits(0);

			m_EffectiveType = Type::Float;
			editor = spin;
		}
		else
		{
			wxSpinCtrl* spin = new wxSpinCtrl(parent, wxID_NONE, wxEmptyString, pos, size, style, m_IntMin, m_IntMax, initialValue);
			spin->SetBase(m_IntBase);

			m_EffectiveType = Type::Integer;
			editor = spin;
		}
	}
	else
	{
		double initialValue = 0.0;
		value.GetAs(&initialValue);

		wxSpinCtrlDouble* spin = new wxSpinCtrlDouble(parent, wxID_NONE, wxEmptyString, pos, size, style, m_FloatMin, m_FloatMax, initialValue, m_FloatIncrement);
		spin->SetDigits(m_FloatPrecision);
		
		m_EffectiveType = Type::Float;
		editor = spin;
	}

	if (HasValidator())
	{
		editor->SetValidator(GetValidator());
	}
	return editor;
}
bool KxDataViewSpinEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	if (m_EffectiveType == Type::Integer)
	{
		value = CastAndGetValue<int>(control);
	}
	else
	{
		// Using float to mimic int
		if (m_Type == Type::Integer)
		{
			value = (int)CastAndGetValue<double>(control);
		}
		else
		{
			value = CastAndGetValue<double>(control);
		}
	}
	return true;
}
