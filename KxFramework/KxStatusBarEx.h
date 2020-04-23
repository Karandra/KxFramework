#pragma once
#include "KxFramework/KxStatusBar.h"
#include "Kx/Drawing/WithImageList.h"
#include "Kx/UI/Controls/IProgressMeter.h"

enum
{
	KxSBE_INHERIT_COLORS = 1 << 0,
	KxSBE_SEPARATORS_ENABLED = 1 << 1,

	KxSBE_MASK = KxSBE_INHERIT_COLORS|KxSBE_SEPARATORS_ENABLED
};

class KX_API KxStatusBarEx:	public KxStatusBar,	public KxFramework::WithImageList, public KxFramework::UI::IProgressMeter
{
	private:
		std::unordered_map<int, int> m_Images;
		wxColour m_ColorBorder = wxNullColour;
		int m_State = wxCONTROL_NONE;
		int m_Style = DefaultStyle;
		bool m_IsSeparatorsVisible = false;

		int m_ProgressPos = 0;
		int m_ProgressRange = 100;
		int m_ProgressStep = 10;
	
	private:
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnMouseDown(wxMouseEvent& event);
		void OnEnter(wxMouseEvent& event);
		void OnLeave(wxMouseEvent& event);
		
		wxEllipsizeMode GetEllipsizeMode() const;
		wxTopLevelWindow* GetTLWParent() const;
		void MakeTopmost();

	protected:
		// KxIProgressBar
		int DoGetRange() const override
		{
			return m_ProgressRange;
		}
		void DoSetRange(int range) override
		{
			m_ProgressRange = range;
			ScheduleRefresh();
		}

		int DoGetValue() const override
		{
			return m_ProgressPos;
		}
		void DoSetValue(int value) override
		{
			m_ProgressPos = value;
			ScheduleRefresh();
		}

		int DoGetStep() const override
		{
			return m_ProgressStep;
		}
		void DoSetStep(int step) override
		{
			m_ProgressStep = step <= m_ProgressRange ? step : m_ProgressRange;
		}

		void DoPulse() override
		{
			ScheduleRefresh();
		}
		bool DoIsPulsing() const override
		{
			return false;
		}

	public:
		static const int DefaultStyle = KxStatusBar::DefaultStyle|KxSBE_SEPARATORS_ENABLED;

		KxStatusBarEx() = default;
		KxStatusBarEx(wxWindow* parent,
					  wxWindowID id,
					  int fieldsCount = 1,
					  long style = DefaultStyle
		)
		{
			Create(parent, id, fieldsCount, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					int fieldsCount = 1,
					long style = DefaultStyle
		);

	public:
		wxColour GetBorderColor() const
		{
			return m_ColorBorder;
		}
		void SetBorderColor(const wxColour& color = wxNullColour)
		{
			m_ColorBorder = color;
			ScheduleRefresh();
		}
		
		bool SetForegroundColour(const wxColour& colour) override;
		bool SetBackgroundColour(const wxColour& color = wxNullColour) override;
		
		bool IsSeparatorsVisible() const
		{
			return m_IsSeparatorsVisible;
		}
		void SetSeparatorsVisible(bool visible = true)
		{
			m_IsSeparatorsVisible = visible;
			ScheduleRefresh();
		}
		
		int GetStatusImage(int index)
		{
			if (index < GetFieldsCount())
			{
				return m_Images[index];
			}
			return -1;
		}
		void SetStatusImage(int imageIndex, int index)
		{
			if (index < GetFieldsCount())
			{
				m_Images[index] = imageIndex;
				ScheduleRefresh();
			}
		}
		
		void SetMinHeight(int height) override;
		void SetStatusText(const wxString& text, int index = 0)
		{
			KxStatusBar::SetStatusText(text, index);
			ScheduleRefresh();
		}

		void SetFieldsCount(int count) override
		{
			KxStatusBar::SetFieldsCount(count);
			ScheduleRefresh();
		}
		void SetFieldsCount(const std::vector<int>& widths) override
		{
			KxStatusBar::SetFieldsCount(widths);
			ScheduleRefresh();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStatusBarEx);
};
