#include "stdafx.h"
#include "SpinEditor.h"
#include "KxFramework/DataView2/Node.h"
#include "Kx/UI/Validator.h"
#include <wx/spinctrl.h>

namespace
{
	template<class T>
	T CastAndGetValue(wxWindow* control)
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

namespace KxDataView2
{
	wxWindow* SpinEditor::CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
	{
		int style = wxSP_ARROW_KEYS|wxTE_PROCESS_ENTER|m_Alignment;
		if (m_IsWrapping)
		{
			style |= wxSP_WRAP;
		}
		if (!IsEditable())
		{
			style |= wxTE_READONLY;
		}

		wxPoint pos = cellRect.GetTopLeft();
		wxSize size = cellRect.GetSize();

		wxControl* editor = nullptr;
		if (m_Type == Type::Integer)
		{
			int initialValue = 0;
			value.GetAs(&initialValue);

			if (ShouldMimicIntegerUsingFloat())
			{
				wxSpinCtrlDouble* spin = new wxSpinCtrlDouble(parent, wxID_NONE, {}, pos, size, style, m_IntMin, m_IntMax, initialValue, m_IntIncrement);
				spin->SetDigits(0);

				m_EffectiveType = Type::Float;
				editor = spin;
			}
			else
			{
				// For some reason 'wxSpinCtrl' created 2 px left relative to given position
				pos.x += 2;

				wxSpinCtrl* spin = new wxSpinCtrl(parent, wxID_NONE, {}, pos, size, style, m_IntMin, m_IntMax, initialValue);
				spin->SetBase(m_IntBase);

				m_EffectiveType = Type::Integer;
				editor = spin;
			}
		}
		else
		{
			double initialValue = 0.0;
			value.GetAs(&initialValue);

			wxSpinCtrlDouble* spin = new wxSpinCtrlDouble(parent, wxID_NONE, {}, pos, size, style, m_FloatMin, m_FloatMax, initialValue, m_FloatIncrement);
			spin->SetDigits(m_FloatPrecision);

			m_EffectiveType = Type::Float;
			editor = spin;
		}
		editor->SetMaxSize(size);

		// Set our win validator ignoring any user supplied because spin controls
		// are different for validators (in other words broken, at least for Windows).
		// See 'KxValidator.cpp' file for details.
		if (m_Type == Type::Integer)
		{
			KxFramework::UI::IntegerValidator<int> validator(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
			validator.SetMin(m_IntMin);
			validator.SetMax(m_IntMax);
			editor->SetValidator(validator);
		}
		else
		{
			KxFramework::UI::FloatingPointValidator<double> validator(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
			validator.SetMin(m_FloatMin);
			validator.SetMax(m_FloatMax);
			editor->SetValidator(validator);
		}
		return editor;
	}
	wxAny SpinEditor::GetValue(wxWindow* control) const
	{
		if (m_EffectiveType == Type::Integer)
		{
			return CastAndGetValue<int>(control);
		}
		else
		{
			// Using float to mimic int
			if (m_Type == Type::Integer)
			{
				return (int)CastAndGetValue<double>(control);
			}
			else
			{
				return CastAndGetValue<double>(control);
			}
		}
		return {};
	}
}
