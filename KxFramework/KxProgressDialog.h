#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxProgressBar.h"

class KxProgressDialog: public KxStdDialog, public KxProgressBarWrapper
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

	public:
		wxDECLARE_DYNAMIC_CLASS(KxProgressDialog);
};
