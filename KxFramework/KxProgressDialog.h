#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxProgressBar.h"

class KX_API KxProgressDialog: public KxStdDialog, public KxProgressBarWrapper
{
	private:
		KxProgressBar* m_View = nullptr;

	private:
		wxOrientation GetViewLabelSizerOrientation() const override;
		bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const override;

	public:
		static const int DefaultProgressWidth = 300;
		static const int DefaultProgressHeight = 16;

		KxProgressDialog() = default;
		KxProgressDialog(wxWindow* parent,
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
		KxProgressBar* GetProgressBar() const
		{
			return m_View;
		}
		KxProgressBar* GetDialogMainCtrl() const override
		{
			return GetProgressBar();
		}
		int GetViewSizerProportion() const override
		{
			return 0;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxProgressDialog);
};
