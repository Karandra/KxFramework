#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxProgressBar.h"

class KxDualProgressDialog: public KxStdDialog
{
	private:
		KxPanel* m_View = NULL;
		KxProgressBar* m_PB1 = NULL;
		KxProgressBar* m_PB2 = NULL;

	private:
		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = NULL) const;
		
		KxProgressBar* CreateProgressBar();

	public:
		static const int DefaultProgressWidth = 300;
		static const int DefaultProgressHeight = 16;

		KxDualProgressDialog() {}
		KxDualProgressDialog(wxWindow* parent,
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
		virtual ~KxDualProgressDialog();

	public:
		KxProgressBar* GetPB1() const
		{
			return m_PB1;
		}
		KxProgressBar* GetPB2() const
		{
			return m_PB2;
		}
		
		virtual wxWindow* GetDialogMainCtrl() const
		{
			return m_View;
		}
		virtual int GetViewSizerProportion() const
		{
			return 0;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDualProgressDialog);
};
