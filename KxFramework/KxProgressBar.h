#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxIProgressBar.h"
class KX_API KxIProgressBarWrapper;

class KX_API KxProgressBar: public wxGauge, public KxIProgressBar
{
	friend class KxProgressBarWrapper;

	private:
		wxString m_Label = "%0.2f%%";
		wxString m_PulseLabel = wxEmptyString;
		int m_Step = DefaultStep;
		bool m_InPulseMode = false;

	private:
		void OnPaint(wxPaintEvent& event);

	protected:
		virtual int DoGetRange() const override
		{
			return wxGauge::GetRange();
		}
		virtual void DoSetRange(int range)
		{
			wxGauge::SetRange(range);
		}

		virtual int DoGetValue() const override
		{
			return wxGauge::GetValue();
		}
		virtual void DoSetValue(int value) override
		{
			m_InPulseMode = false;
			wxGauge::SetValue(value);
		}

		virtual int DoGetStep() const override
		{
			return m_Step;
		}
		virtual void DoSetStep(int step) override
		{
			const int range = DoGetRange();
			m_Step = step <= range ? step : range;
		}

		virtual void DoPulse() override
		{
			if (!m_InPulseMode)
			{
				wxGauge::Pulse();
			}
			m_InPulseMode = true;
		}
		virtual bool DoIsPulsing() const override
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

		// Resolve ambiguity
		virtual void Pulse() override
		{
			KxIProgressBar::Pulse();
		}

		int GetRange() const
		{
			return KxIProgressBar::GetRange();
		}
		void SetRange(int range)
		{
			KxIProgressBar::SetRange(range);
		}

		int GetValue() const
		{
			return KxIProgressBar::GetValue();
		}
		void SetValue(int value)
		{
			KxIProgressBar::SetValue(value);
		}
		void SetValue(int64_t current, int64_t max)
		{
			KxIProgressBar::SetValue(current, max);
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxProgressBar);
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxProgressBarWrapper: public KxIProgressBar
{
	private:
		KxProgressBar* m_ProgressBar = NULL;

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
		virtual int DoGetRange() const override
		{
			return m_ProgressBar->DoGetRange();
		}
		virtual void DoSetRange(int range) override
		{
			m_ProgressBar->DoSetRange(range);
		}

		virtual int DoGetValue() const override
		{
			return m_ProgressBar->DoGetValue();
		}
		virtual void DoSetValue(int value) override
		{
			m_ProgressBar->DoSetValue(value);
		}

		virtual int DoGetStep() const override
		{
			return m_ProgressBar->DoGetStep();
		}
		virtual void DoSetStep(int step) override
		{
			m_ProgressBar->DoSetStep(step);
		}

		virtual void DoPulse() override
		{
			m_ProgressBar->DoPulse();
		}
		virtual bool DoIsPulsing() const override
		{
			return m_ProgressBar->DoIsPulsing();
		}

	public:
		KxProgressBarWrapper(KxProgressBar* progressBar = NULL)
			:m_ProgressBar(progressBar)
		{
		}
};
