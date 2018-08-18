#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxProgressBar.h"

class KxProgressDialog: public KxStdDialog
{
	private:
		KxProgressBar* m_View = NULL;

	private:
		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = NULL) const;

	public:
		static const int DefaultProgressWidth = 300;
		static const int DefaultProgressHeight = 16;

		KxProgressDialog() {}
		KxProgressDialog(wxWindow* parent,
						  wxWindowID id,
						  const wxString& caption,
						  const wxPoint& pos = wxDefaultPosition,
						  const wxSize& size = wxDefaultSize,
						  int buttons = DefaultButtons,
						  long style = DefaultStyle
		)
		{
			Create(parent, id, caption, pos, size, buttons, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& caption,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					int buttons = DefaultButtons,
					long style = DefaultStyle
		);
		virtual ~KxProgressDialog();

	public:
		KxProgressBar* GetProgressBar() const
		{
			return m_View;
		}
		virtual KxProgressBar* GetDialogMainCtrl() const override
		{
			return GetProgressBar();
		}
		virtual int GetViewSizerProportion() const override
		{
			return 0;
		}
		virtual void ResetState() override;

		int GetRange() const
		{
			return GetProgressBar() ? GetProgressBar()->GetRange() : 0;
		}
		void SetRange(int max)
		{
			if (GetProgressBar())
			{
				GetProgressBar()->SetRange(max);
			}
		}
		
		int GetStep() const
		{
			return GetProgressBar() ? GetProgressBar()->GetStep() : 0;
		}
		void SetStep(int value)
		{
			if (GetProgressBar())
			{
				GetProgressBar()->SetStep(value);
			}
		}
		
		int GetValue() const
		{
			return GetProgressBar() ? GetProgressBar()->GetValue() : 0;
		}
		void SetValue(int value)
		{
			if (GetProgressBar())
			{
				GetProgressBar()->SetValue(value);
			}
		}
		void SetValue(int64_t current, int64_t max)
		{
			if (GetProgressBar())
			{
				GetProgressBar()->SetValue(current, max);
			}
		}
		
		void Advance(int value)
		{
			if (GetProgressBar())
			{
				GetProgressBar()->Advance(value);
			}
		}
		void Pulse()
		{
			if (GetProgressBar())
			{
				GetProgressBar()->Pulse();
			}
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxProgressDialog);
};
