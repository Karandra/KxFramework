#pragma once
#include "KxFramework/KxFramework.h"

class KxProgressBar: public wxGauge
{
	private:
		wxString m_Label = "%0.2f%%";
		wxString m_PulseLabel = wxEmptyString;
		int m_Step = DefaultStep;
		bool m_InPulseMode = false;

	private:
		void OnPaint(wxPaintEvent& hEvent);

	public:
		static const long DefaultStyle = wxGA_HORIZONTAL|wxGA_SMOOTH;
		static const int DefaultStep = 10;
		static const int DefaultRange = 100;

		KxProgressBar() {}
		KxProgressBar(wxWindow* parent,
					  wxWindowID id,
					  int range = DefaultRange,
					  long style = DefaultStyle
		)
		{
			Create(parent, id, range, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					int range = DefaultRange,
					long style = DefaultStyle
		);

	public:
		virtual void Pulse() override;
		virtual void SetValue(int pos) override;
		void SetValue(int64_t current, int64_t max);

		int GetStep() const
		{
			return m_Step;
		}
		void SetStep(int step)
		{
			m_Step = step <= GetRange() ? step : GetRange();
		}
		void Advance(int value);
		void StepIt()
		{
			Advance(GetStep());
		}

		virtual wxString GetLabel() const override
		{
			return m_Label;
		}
		virtual void SetLabel(const wxString& label) override
		{
			m_Label = label;
			Refresh();
		}
		
		const wxString& GetPulseLabel() const
		{
			return m_PulseLabel;
		}
		void SetPulseLabel(const wxString& label)
		{
			m_PulseLabel = label;
			Refresh();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxProgressBar);
};
