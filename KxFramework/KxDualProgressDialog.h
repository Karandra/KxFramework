#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxProgressBar.h"

class KX_API KxDualProgressDialog: public KxStdDialog
{
	private:
		KxPanel* m_View = nullptr;
		KxProgressBar* m_PB1 = nullptr;
		KxProgressBar* m_PB2 = nullptr;

	private:
		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const;
		
		KxProgressBar* CreateProgressBar();

	public:
		static const int DefaultProgressWidth = 300;
		static const int DefaultProgressHeight = 16;

		KxDualProgressDialog() = default;
		KxDualProgressDialog(wxWindow* parent,
							 wxWindowID id,
							 const wxString& caption,
							 const wxPoint& pos = wxDefaultPosition,
							 const wxSize& size = wxDefaultSize,
							 StdButton buttons = DefaultButtons,
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
					StdButton buttons = DefaultButtons,
					long style = DefaultStyle
		);

	public:
		KxProgressBar* GetPB1() const
		{
			return m_PB1;
		}
		KxProgressBar* GetPB2() const
		{
			return m_PB2;
		}
		
		wxWindow* GetDialogMainCtrl() const override
		{
			return m_View;
		}
		int GetViewSizerProportion() const override
		{
			return 0;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDualProgressDialog);
};
