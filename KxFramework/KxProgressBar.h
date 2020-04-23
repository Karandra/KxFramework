#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/UI/Controls/IProgressMeter.h"
class KX_API KxIProgressBarWrapper;

class KX_API KxProgressBar: public wxGauge, public KxFramework::UI::IProgressMeter
{
	friend class KxProgressBarWrapper;

	private:
		wxString m_Label = "%0.2f%%";
		wxString m_PulseLabel = {};
		int m_Step = DefaultStep;
		bool m_InPulseMode = false;

	private:
		void OnPaint(wxPaintEvent& event);

	protected:
		int DoGetRange() const override
		{
			return wxGauge::GetRange();
		}
		void DoSetRange(int range) override
		{
			wxGauge::SetRange(range);
		}

		int DoGetValue() const override
		{
			return wxGauge::GetValue();
		}
		void DoSetValue(int value) override
		{
			m_InPulseMode = false;
			wxGauge::SetValue(value);
		}

		int DoGetStep() const override
		{
			return m_Step;
		}
		void DoSetStep(int step) override
		{
			const int range = DoGetRange();
			m_Step = step <= range ? step : range;
		}

		void DoPulse() override
		{
			if (!m_InPulseMode)
			{
				wxGauge::Pulse();
			}
			m_InPulseMode = true;
		}
		bool DoIsPulsing() const override
		{
			return m_InPulseMode;
		}

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
		wxString GetLabel() const override
		{
			return m_Label;
		}
		void SetLabel(const wxString& label) override
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

		// Resolve ambiguity
		void Pulse() override
		{
			IProgressMeter::Pulse();
		}

		int GetRange() const
		{
			return IProgressMeter::GetRange();
		}
		void SetRange(int range)
		{
			IProgressMeter::SetRange(range);
		}

		int GetValue() const
		{
			return IProgressMeter::GetValue();
		}
		void SetValue(int value)
		{
			IProgressMeter::SetValue(value);
		}
		void SetValue(int64_t current, int64_t max)
		{
			IProgressMeter::SetValue(current, max);
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxProgressBar);
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxProgressBarWrapper: public KxFramework::UI::IProgressMeter
{
	private:
		KxProgressBar* m_ProgressBar = nullptr;

	protected:
		KxProgressBar* GetProgressBar() const
		{
			return m_ProgressBar;
		}
		void SetProgressBar(KxProgressBar* progressBar)
		{
			m_ProgressBar = progressBar;
		}

	protected:
		int DoGetRange() const override
		{
			return m_ProgressBar->DoGetRange();
		}
		void DoSetRange(int range) override
		{
			m_ProgressBar->DoSetRange(range);
		}

		int DoGetValue() const override
		{
			return m_ProgressBar->DoGetValue();
		}
		void DoSetValue(int value) override
		{
			m_ProgressBar->DoSetValue(value);
		}

		int DoGetStep() const override
		{
			return m_ProgressBar->DoGetStep();
		}
		void DoSetStep(int step) override
		{
			m_ProgressBar->DoSetStep(step);
		}

		void DoPulse() override
		{
			m_ProgressBar->DoPulse();
		}
		bool DoIsPulsing() const override
		{
			return m_ProgressBar->DoIsPulsing();
		}

	public:
		KxProgressBarWrapper(KxProgressBar* progressBar = nullptr)
			:m_ProgressBar(progressBar)
		{
		}
};
