#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"

class KxStdDialogSimple: public KxStdDialog
{
	private:
		wxWindow* m_View = NULL;

	private:
		virtual wxOrientation GetWindowResizeSide() const
		{
			return wxBOTH;
		}

	public:
		static const long DefaultStyle = KxStdDialog::DefaultStyle;

		KxStdDialogSimple() {}
		bool Create(wxWindow* parent,
					wxWindowID id,
					wxWindow* pView,
					const wxString& caption,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					int buttons = DefaultButtons,
					long style = DefaultStyle
		);
		virtual ~KxStdDialogSimple();

	public:
		virtual wxWindow* GetDialogMainCtrl() const override
		{
			return m_View;
		}
		virtual int GetViewSizerProportion() const
		{
			return 1;
		}
		virtual void ResetState()
		{
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStdDialogSimple);
};
