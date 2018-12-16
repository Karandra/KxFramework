#pragma once
#include "KxFramework/KxFramework.h"

enum
{
	KxSL_HORIZONTAL = wxSL_HORIZONTAL,
	KxSL_VERTICAL = wxSL_VERTICAL,

	KxSL_TICKS = wxSL_TICKS,
	KxSL_AUTOTICKS = wxSL_AUTOTICKS,
	KxSL_NO_THUMB = 32, // 32 is free

	KxSL_LEFT = wxSL_LEFT,
	KxSL_TOP = wxSL_TOP,
	KxSL_RIGHT = wxSL_RIGHT,
	KxSL_BOTTOM = wxSL_BOTTOM,

	KxSL_BOTH = wxSL_BOTH,
	KxSL_SELRANGE = wxSL_SELRANGE,
	KxSL_INVERSE = wxSL_INVERSE,
	KxSL_MIN_MAX_LABELS = wxSL_MIN_MAX_LABELS,
	KxSL_VALUE_LABEL = wxSL_VALUE_LABEL,
	KxSL_LABELS = wxSL_LABELS,
};

class KX_API KxSlider: public wxSlider
{
	private:
		int m_Step = DefaultStep;

	public:
		static const long DefaultStyle = KxSL_HORIZONTAL;
		static const int DefaultStep = 10;

		KxSlider() {}
		KxSlider(wxWindow* parent,
				 wxWindowID id,
				 int value,
				 int minValue,
				 int maxValue,
				 long style = DefaultStyle,
				 const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, value, minValue, maxValue, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					int value,
					int minValue,
					int maxValue,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);

	public:
		bool IsInversed() const
		{
			return HasFlag(wxSL_INVERSE);
		}
		int GetSelStart() const
		{
			if (IsInversed())
			{
				return wxSlider::GetMax() - wxSlider::GetSelEnd();
			}
			else
			{
				return wxSlider::GetSelStart();
			}
		}
		int GetSelEnd() const
		{
			if (IsInversed())
			{
				return wxSlider::GetMax() - wxSlider::GetSelStart();
			}
			else
			{
				return wxSlider::GetSelEnd();
			}
		}
		void SetSelection(int min, int max)
		{
			if (IsInversed())
			{
				wxSlider::SetSelection(wxSlider::GetMax() - max, wxSlider::GetMax() - min);
			}
			else
			{
				wxSlider::SetSelection(min, max);
			}
		}

		int GetStep() const
		{
			return m_Step;
		}
		void SetStep(int step)
		{
			m_Step = step;
		}
		void Advance(int value)
		{
			int max = GetMax();
			value += GetValue();
			if (value <= max)
			{
				SetValue(value);
			}
			else
			{
				SetValue(max);
			}
		}
		void StepIt()
		{
			Advance(GetStep());
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxSlider);
};
